import json
import matplotlib.pyplot as plt
import pandas as pd
import argparse
import os

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
    Parsa il nome del benchmark complesso che hai costruito.
    Restituisce un dizionario di parametri.
    DA ADATTARE ALLA TUA STRUTTURA ESATTA DEL NOME!
    Esempio di nome: "int32_AlphaDev_Intro_maxSmall1_rngFullType_ordRandom/1000/1"
                     "int32_AlphaDev_Intro_maxSmall1_rngFullType_ordRandom/Args_0:1000/Args_1:1" (dipende dalla versione di gbenchmark)
    """
    params = {}
    parts = name_str.split('/')
    base_name_parts = parts[0].split('_')

    # Estrarre N e maxSmall dagli Args se presenti
    if len(parts) > 1 and 'Args_0:' in parts[1] and 'Args_1:' in parts[2] : #Formato con Args_X:Y
        params[PARAM_N] = int(parts[1].split(':')[1])
        params[PARAM_MAX_SMALL] = int(parts[2].split(':')[1])
    elif len(parts) > 1 and len(parts) == 3 : # Formato BM_Name/N/maxSmall
         params[PARAM_N] = int(parts[1])
         params[PARAM_MAX_SMALL] = int(parts[2])


    # Esempio basato sulla tua stringa di nome (ADATTA QUESTO!)
    # "int32_AlphaDev_Intro_maxSmall1_rngFullType_ordRandom"
    try:
        params[PARAM_TYPE] = base_name_parts[0]
        params[PARAM_SS] = base_name_parts[1]
        params[PARAM_ALG] = base_name_parts[2]
        # maxSmall nel nome potrebbe essere ridondante se lo prendi da Args_1, ma utile per il filtro iniziale
        if "maxSmall" in base_name_parts[3]:
             # params[PARAM_MAX_SMALL] = int(base_name_parts[3].replace("maxSmall", "")) # Sovrascritto da Args se presente
             pass # Già gestito da Args
        params[PARAM_RANGE_TYPE] = base_name_parts[4].replace("rng", "")
        params[PARAM_ORDER_TYPE] = base_name_parts[5].replace("ord", "")
    except IndexError:
        print(f"Attenzione: impossibile parsare completamente il nome: {parts[0]}")
        # Potresti voler gestire diversamente i benchmark che non seguono questo pattern
        # (es. std::sort che non ha ss o maxSmall)


    # Gestione di std::sort e algoritmi semplici che non hanno tutti i parametri
    if params.get(PARAM_ALG) == "StdSort" or \
       params.get(PARAM_ALG) in ["Selection", "Insertion", "Bubble"]:
        params[PARAM_SS] = "N/A" # Non applicabile
        if params.get(PARAM_ALG) == "StdSort": # StdSort non ha maxSmall come lo intendi tu
             if PARAM_MAX_SMALL not in params: # Se non preso da Args
                params[PARAM_MAX_SMALL] = "N/A"


    # Se N o maxSmall non sono stati presi da Args, segnala un problema o usa un default
    if PARAM_N not in params:
        # Questo non dovrebbe accadere se usi ->Args({N, ms})
        print(f"Attenzione: N non trovato per {name_str}")
        params[PARAM_N] = 0 # o gestisci l'errore
    if PARAM_MAX_SMALL not in params and params.get(PARAM_SS) != "N/A":
        # Se non è N/A e non è stato trovato, potrebbe essere un problema
        # per algoritmi che lo usano. Per quelli che non lo usano (es. HeapSort), va bene
        if params.get(PARAM_ALG) not in ["Heap", "Selection", "Insertion", "Bubble", "StdSort"]:
            print(f"Attenzione: maxSmall non trovato per {name_str}")
            params[PARAM_MAX_SMALL] = 0 # o gestisci


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

    plt.figure(figsize=(12, 7))
    
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
            # Assicurati che x_param sia numerico per il plot e ordinato
            group = group.sort_values(by=x_param)
            plt.plot(group[x_param], group[y_param], marker='o', linestyle='-', label=label)
    else: # Nessun parametro variante, solo una linea
        filtered_df = filtered_df.sort_values(by=x_param)
        plt.plot(filtered_df[x_param], filtered_df[y_param], marker='o', linestyle='-')

    plt.xlabel(x_param.replace('_',' ').title())
    y_label_base = y_param.replace('_',' ').title()
    time_unit = filtered_df['time_unit'].iloc[0] if not filtered_df.empty and 'time_unit' in filtered_df.columns else "unknown_unit"
    plt.ylabel(f"{y_label_base} ({time_unit})")
    
    # Mostra i fixed_params come titolo sopra il grafico
    if fixed_params:
        fixed_str = ", ".join(f"{k}={v}" for k, v in fixed_params.items())
        plt.title(f"Parametri fissi: {fixed_str}", fontsize=11)
    plt.legend(loc='best', fontsize='small')
    plt.grid(True, which="both", ls="--", alpha=0.7)
    plt.xscale('log') # Spesso utile per l'asse N
    plt.yscale('log') # Spesso utile per i tempi

    filename = f"{filename_base}"
    plt.savefig(filename)
    plt.close()
    print(f"Grafico salvato: {filename}")

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
            PARAM_RANGE_TYPE: "FullType",
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