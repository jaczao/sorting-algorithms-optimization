import json
import matplotlib.pyplot as plt
import pandas as pd
import argparse
import os
import matplotlib.ticker as mticker

# Nomi dei parametri come appaiono nel JSON di Google Benchmark (derivati dai tuoi nomi C++)
# Google Benchmark aggiunge "BM_All/" al nome del benchmark e i parametri Args vengono aggiunti dopo.
# Il nome che costruisci tu (es. "int32_AlphaDev_Intro_maxSmall1_rngFullType_ordRandom")
# diventa la parte principale del 'name' nel JSON.
# I tuoi Args({N, ms}) diventano campi come 'Args_0' (per N) e 'Args_1' (per maxSmall).

# Mappature per leggibilità (DA ADATTARE AI TUOI NOMI ESATTI NEL JSON!)
# Questi sono i nomi che hai dato tu nella stringa 'name' nel C++
PARAM_TYPE = "type"
PARAM_SS = "small_sort_strategy"
PARAM_ALG = "algorithm"
PARAM_MAX_SMALL = "max_small"
PARAM_RANGE_TYPE = "range_type"
PARAM_ORDER_TYPE = "order_type"
PARAM_N = "N" # Questo sarà 'Args_0' o simile, o lo estrarremo dal nome

# Asse Y di default
METRIC_Y = 'cpu_time' # o 'real_time'

def parse_benchmark_name(name_str):
    """
    Parsa il nuovo nome del benchmark in formato chiave:valore, separato da virgole, seguito da /N.
    Esempio: "type:int32,smallSort:AlphaDev,algo:MergeSort,maxSmall:64,range:Full,order:NaturalRuns/1000000"
    Restituisce un dizionario di parametri.
    """
    params = {}
    # Split tra parte parametri e N
    if '/' in name_str:
        param_part, n_part = name_str.split('/', 1)
        try:
            params[PARAM_N] = int(n_part)
        except ValueError:
            print(f"Attenzione: N non numerico in {name_str}")
            params[PARAM_N] = 0
    else:
        param_part = name_str
        params[PARAM_N] = 0
    # Split parametri chiave:valore
    for pair in param_part.split(','):
        if ':' in pair:
            k, v = pair.split(':', 1)
            k = k.strip()
            v = v.strip()
            # Mappa le chiavi ai parametri interni
            if k == 'type':
                params[PARAM_TYPE] = v
            elif k == 'smallSort':
                params[PARAM_SS] = v
            elif k == 'algo':
                params[PARAM_ALG] = v
            elif k == 'maxSmall':
                try:
                    params[PARAM_MAX_SMALL] = int(v)
                except ValueError:
                    params[PARAM_MAX_SMALL] = v
            elif k == 'range':
                params[PARAM_RANGE_TYPE] = v
            elif k == 'order':
                params[PARAM_ORDER_TYPE] = v
            else:
                params[k] = v
    # Default per maxSmall se non presente e serve
    if PARAM_MAX_SMALL not in params and params.get(PARAM_SS) != "N/A":
        if params.get(PARAM_ALG) not in ["Heap", "Selection", "Insertion", "Bubble", "StdSort"]:
            params[PARAM_MAX_SMALL] = 1
    return params

def load_data(json_file):
    """Carica i dati dal file JSON di Google Benchmark e li elabora."""
    with open(json_file, 'r') as f:
        data = json.load(f)

    processed_benchmarks = []
    for bm in data['benchmarks']:
        name = bm['name']
        params = parse_benchmark_name(name)
        
        # Aggiungi i parametri parsati e le metriche al nostro record
        record = {**params} # Unisci i parametri base
        record['name_original'] = name
        record[METRIC_Y] = bm[METRIC_Y] # Assumendo che 'cpu_time' sia sempre presente
        record['time_unit'] = bm['time_unit']
        # Aggiungi 'N' e 'max_small' presi da Args (se non già fatto da parse_benchmark_name in modo robusto)
        if 'Args_0' in bm: record[PARAM_N] = bm['Args_0'] # Se usi Args per N
        if 'Args_1' in bm: record[PARAM_MAX_SMALL] = bm['Args_1'] # Se usi Args per maxSmall

        processed_benchmarks.append(record)
    
    return pd.DataFrame(processed_benchmarks)

# ... (il codice prima di plot_benchmark rimane invariato) ...


# ... (tutto il codice precedente a plot_benchmark rimane invariato) ...

def plot_benchmark(df, x_param, fixed_params, varying_params, filename_base):
    """
    Genera un grafico.
    - df: DataFrame con i dati.
    - x_param: Nome della colonna per l'asse X (es. PARAM_N).
    - y_param: Nome della colonna per l'asse Y (es. METRIC_Y).
    - fixed_params: Dizionario dei parametri da fissare e i loro valori.
    - varying_params: Lista dei nomi dei parametri che genereranno linee diverse nel grafico.
    - filename_base: Path completo del file del grafico (senza estensione).
    """
    y_param = METRIC_Y  # cpu_time fisso

    # Filtra i dati in base ai parametri fissi
    query_parts = []
    for key, value in fixed_params.items():
        if isinstance(value, str):
            query_parts.append(f"`{key}` == '{value}'")
        else: # Assumiamo numerico
            query_parts.append(f"`{key}` == {value}")
    
    filtered_df = df
    if query_parts:
        query = " & ".join(query_parts)
        try:
            filtered_df = df.query(query)
        except Exception as e:
            print(f"Errore nel filtrare i dati con query: {query}")
            print(f"DataFrame columns: {df.columns.tolist()}")
            print(f"Fixed params: {fixed_params}")
            print(f"Exception: {e}")
            return

    if filtered_df.empty:
        print(f"Nessun dato trovato per i parametri fissati: {fixed_params}. Salto il grafico.")
        return

    # MODIFICA: Figura Quadrata (era nel suggerimento precedente, lo includo qui)
    plt.figure(figsize=(10, 7.5)) # Puoi aggiustare la dimensione
    ax = plt.gca() # Ottieni l'oggetto Axes corrente
    
    # Raggruppa per i parametri che variano per creare linee diverse
    # Costruisci una legenda dinamica
    if varying_params:
        grouped = filtered_df.groupby(varying_params)
        for name_tuple, group in grouped:
            label_parts = []
            if isinstance(name_tuple, tuple): # Multipli parametri varianti
                for i, p_name in enumerate(varying_params):
                    label_parts.append(f"{p_name.replace('_',' ').title()}: {name_tuple[i]}")
            else: # Singolo parametro variante
                 label_parts.append(f"{varying_params[0].replace('_',' ').title()}: {name_tuple}")
            
            label = ", ".join(label_parts)
            group = group.sort_values(by=x_param)
            ax.plot(group[x_param], group[y_param], marker='o', linestyle='-', label=label) # Usa ax.plot
    else: # Nessun parametro variante, solo una linea
        filtered_df = filtered_df.sort_values(by=x_param)
        ax.plot(filtered_df[x_param], filtered_df[y_param], marker='o', linestyle='-') # Usa ax.plot

    ax.set_xlabel(x_param.replace('_',' ').title()) # Usa ax.set_xlabel
    y_label_base = y_param.replace('_',' ').title()
    time_unit = filtered_df['time_unit'].iloc[0] if not filtered_df.empty and 'time_unit' in filtered_df.columns else "unknown_unit"
    ax.set_ylabel(f"{y_label_base} ({time_unit})") # Usa ax.set_ylabel
    
    if fixed_params:
        fixed_str = ", ".join(f"{k}={v}" for k, v in fixed_params.items())
        ax.set_title(f"Parametri fissi: {fixed_str}", fontsize=11) # Usa ax.set_title
    
    ax.legend(loc='best', fontsize='11') # Usa ax.legend
    
    # MODIFICA: Griglia Personalizzata (era nel suggerimento precedente)
    ax.grid(True, which='major', linestyle='--', linewidth=0.7, alpha=0.7)

    # Imposta la scala dell'asse x e y
    x_unique_values = sorted(filtered_df[x_param].unique())
    is_x_log_scale = False
    if len(x_unique_values) > 1:
        if x_unique_values[-1] > 8:
            ax.set_xscale('log')
            is_x_log_scale = True

    # Mostra solo i tick corrispondenti agli x_unique_values, sia in scala normale che log
    ax.set_xticks(x_unique_values)
    ax.set_xticklabels([str(val) for val in x_unique_values])
    ax.minorticks_off()

    if filtered_df[y_param].nunique() > 1:
        ax.set_yscale('log')

    filename = f"{filename_base}" 
    plt.tight_layout() # Aggiunto per migliorare il layout, specialmente con etichette ruotate
    plt.savefig(filename)
    plt.close()
    print(f"Grafico salvato: {filename}")

# ... (il resto del codice, inclusa la funzione main e generate_all_plots, rimane invariato) ...

# --- Definizione dei Grafici da Generare ---
def generate_all_plots(df):
    # Esempio 1: Confronto Algoritmi per Int32, Random, FullRange, maxSmall fisso
    # Asse X: N, Linee: Algoritmo
    plot_benchmark(
        df=df,
        x_param=PARAM_N,
        fixed_params={
            PARAM_TYPE: "int32",
            PARAM_ORDER_TYPE: "Random",
            PARAM_RANGE_TYPE: "Full",
        },
        varying_params=[PARAM_ALG, PARAM_MAX_SMALL], # Algoritmi diversi, una linea per ciascuno
        filename_base="alg_comparison_int32_random_full_ms16"
    )


    print("--- Generazione Grafici Completata ---")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Genera un grafico dai risultati di Google Benchmark.")
    parser.add_argument("json_file", help="Path al file JSON di output di Google Benchmark.")
    parser.add_argument("output_filename_base", help="Path completo (senza estensione) del file di output.")
    parser.add_argument("--x_param", required=True, help="Parametro per l'asse X (es: N, max_small, ...)")
    parser.add_argument("--varying_params", default="", help="Parametri separati da virgola che generano linee diverse (es: algorithm,range_type)")
    parser.add_argument("--fixed_params", default="", help="Parametri fissati in formato chiave=valore,chiave=valore (es: type=int32,order_type=Random)")
    args = parser.parse_args()

    dataframe = load_data(args.json_file)

    # Parsing fixed_params
    fixed_params = {}
    if args.fixed_params:
        for kv in args.fixed_params.split(','):
            if '=' in kv:
                k, v = kv.split('=', 1)
                try:
                    v = int(v)
                except ValueError:
                    pass
                fixed_params[k] = v

    # Parsing varying_params
    varying_params = [p for p in args.varying_params.split(',') if p] if args.varying_params else []

    # Usa direttamente l'argomento fornito come base del filename
    filename_base = args.output_filename_base

    plot_benchmark(
        df=dataframe,
        x_param=args.x_param,
        fixed_params=fixed_params,
        varying_params=varying_params,
        filename_base=filename_base
    )