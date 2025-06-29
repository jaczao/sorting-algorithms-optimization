#!/usr/bin/env bash
set -euo pipefail

# 1) Genera i grafici
source venv_grafici/Scripts/activate

#python ./benchmarks/generate_graphs.py ./outputs/results.json

python benchmarks/generate_graphs.py ./outputs/results1.json ./plots_win/plot1.png --x_param N --fixed_params type=int32,order_type=Random,max_small=1,small_sort_strategy=AlphaDev --varying_params algorithm,range_type

python benchmarks/generate_graphs.py ./outputs/results2.json ./plots_win/plot2.png --x_param max_small --fixed_params type=int32,order_type=Random,range_type=Full,N=10000 --varying_params algorithm,small_sort_strategy

python benchmarks/generate_graphs.py ./outputs/results3.json ./plots_win/plot3.png --x_param max_small --fixed_params type=int32,order_type=Random,range_type=Full,N=10000 --varying_params algorithm,small_sort_strategy

python benchmarks/generate_graphs.py ./outputs/results3.json ./plots_win/plot4.png --x_param max_small --fixed_params type=int64,order_type=Random,range_type=Full,N=10000 --varying_params algorithm,small_sort_strategy

python benchmarks/generate_graphs.py ./outputs/results3.json ./plots_win/plot5.png --x_param max_small --fixed_params type=float,order_type=Random,range_type=Full,N=10000 --varying_params algorithm,small_sort_strategy

python benchmarks/generate_graphs.py ./outputs/results4.json ./plots_win/plot6.png --x_param N --fixed_params type=int32,order_type=Random,range_type=Full,algorithm=TimSort --varying_params small_sort_strategy,max_small

python benchmarks/generate_graphs.py ./outputs/results5.json ./plots_win/plot7.png --x_param N --fixed_params type=int32,order_type=Random,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results5.json ./plots_win/plot8.png --x_param N --fixed_params type=int64,order_type=Random,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results5.json ./plots_win/plot9.png --x_param N --fixed_params type=float,order_type=Random,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results6.json ./plots_win/plot10.png --x_param N --fixed_params type=int32,order_type=Ascending,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results6.json ./plots_win/plot11.png --x_param N --fixed_params type=int32,order_type=Descending,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results6.json ./plots_win/plot12.png --x_param N --fixed_params type=int32,order_type=PipeOrgan,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results6.json ./plots_win/plot13.png --x_param N --fixed_params type=int32,order_type=Constant,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results6.json ./plots_win/plot14.png --x_param N --fixed_params type=int32,order_type=MostlySorted,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

python benchmarks/generate_graphs.py ./outputs/results6.json ./plots_win/plot15.png --x_param N --fixed_params type=int32,order_type=NaturalRuns,range_type=Full,small_sort_strategy=BranchlessSN --varying_params algorithm,max_small

deactivate
