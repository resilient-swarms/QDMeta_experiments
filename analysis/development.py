
# behavioural metrics defined in Mouret, J., & Clune, J. (2015). Illuminating search spaces by mapping elites. 1–15.
import os
import operator

import argparse

from process_archive import *
from filenames import *

from plots import *
import copy

from check_metagens import check_metagens_forfuneval
PRINT=False

#LAMBDA=5
#max_gen_dict = {"damage_meta": 260, "envir_meta": 270}
max_gen_dict = {"default": 1000}
parser = argparse.ArgumentParser(description='Process destination folder.')
parser.add_argument('-d', dest='DEST', type=str)

args = parser.parse_args()

if args.DEST.endswith("RHEX"):
    MAX_FUN_EVALS=12* 10 ** 6
else:
    MAX_FUN_EVALS = 100 * 10 ** 6
BATCH_SIZE=400
BY_FUNEVAL=False
RUNNING_AVERAGE=True

def get_max_metagen(BD_directory,condition,run,step):
    metagen=0
    maxgen=0
    while True:
        try:
            a,dir = get_archive_plus_dir(BD_directory, condition, metagen, index=0)
            file=open(dir+run+"/"+a,"rb")
        except:
            return maxgen
        maxgen=metagen
        metagen+=step
    raise Exception("loop cannot be stopped without exception")
def get_timebins(BD_directory,condition,max_gen, bins,step):
    """ for a given max_gen, return evenly spaced bins, given a stepsize of 100 in the metageneration files"""
    evenly_spaced = np.linspace(0,max_gen, bins)  # ideal case we will approximate
    timebins=[]
    for ideal_bin in evenly_spaced:
        # divide by step, round, multiply back by step
        closest_match = int(round(ideal_bin/step) * step)
        try:
            _a,_dir = get_archive_plus_dir(BD_directory, condition, closest_match, index=0)
        except:
            raise Exception("file ", _a, " not found")
        timebins.append(closest_match)
    return timebins

def print_conditional(some_string):
    if PRINT:
        print(some_string)

def get_all_performances(bd_dims, path,conversion_func=None,from_fitfile=False):
    if conversion_func is not None:
        all_performances = [conversion_func(fitness) for fitness in get_bin_performances_uniquearchive(path,bd_dims,from_fitfile).values()]
    else:
        all_performances = [fitness for fitness in get_bin_performances_uniquearchive(path,bd_dims,from_fitfile).values()]
    return all_performances

def global_performances(bd_dims, BD_directory,  runs, archive_file_paths, max_performance,conversion_func,from_fitfile=False):
    stats = []
    if not isinstance(archive_file_paths,list):
        archive_file_paths=[archive_file_paths]
    for path in archive_file_paths:
        for run in runs:
            stats.append(_global_performance(bd_dims,  BD_directory, run, path, max_performance,conversion_func,from_fitfile))

    print_conditional("global performances: " + str(stats))
    return stats

def _global_performance( bd_dims, BD_directory, run,archive_file_path,max_performance,conversion_func=None,from_fitfile=False):
    """
    For each run, the single highest- performing
    solution found by that algorithm anywhere in the search space
    divided by the highest performance possi- ble in that domain.
    If it is not known what the maximum theoretical performance is,
    as is the case for all of our do- mains,
    it can be estimated by dividing by the highest performance found by any algorithm in any run. ( BUT avoid estimation if you can)
    This measure is the traditional, most common way to evaluate optimization algorithms.
    This measure is the traditional, most common way to evaluate optimization algorithms.
    One can also measure whether any illumination algorithm also performs well on this measurement.
    Both the ideal optimization algorithm and the ideal illumination
    algorithm are expected to perform perfectly on this measure

    :param BD_directory: directory in which all the runs of a BD are located,
            e.g. ~/Desktop/history_obstacles
    :param experiment_file_path: relative path from the BD_directory to the archive file
    """
    path=get_archive_filepath(BD_directory, run, archive_file_path)
    all_performances = get_all_performances(bd_dims, path,  conversion_func,from_fitfile)
    return max(all_performances)/max_performance


def avg_performances(bd_dims, BD_directory,  runs, archive_file_paths, max_performance,conversion_func,from_fitfile):
    stats = []
    if not isinstance(archive_file_paths,list):
        archive_file_paths=[archive_file_paths]
    for path in archive_file_paths:
        for run in runs:
            stats.append(_avg_performance(bd_dims, BD_directory,  run, path, max_performance,conversion_func,from_fitfile))
    print_conditional("avg performances: " + str(stats))
    return stats

def _avg_performance(bd_dims, BD_directory,  run,archive_file_path,max_performance,conversion_func=None,from_fitfile=False):
    """
    For each run, the single highest- performing
    solution found by that algorithm anywhere in the search space
    divided by the highest performance possi- ble in that domain.
    If it is not known what the maximum theoretical performance is,
    as is the case for all of our do- mains,
    it can be estimated by dividing by the highest performance found by any algorithm in any run. ( BUT avoid estimation if you can)
    This measure is the traditional, most common way to evaluate optimization algorithms.
    This measure is the traditional, most common way to evaluate optimization algorithms.
    One can also measure whether any illumination algorithm also performs well on this measurement.
    Both the ideal optimization algorithm and the ideal illumination
    algorithm are expected to perform perfectly on this measure

    :param BD_directory: directory in which all the runs of a BD are located,
            e.g. ~/Desktop/history_obstacles
    :param experiment_file_path: relative path from the BD_directory to the archive file
    """
    path=get_archive_filepath(BD_directory,run, archive_file_path)
    all_performances=get_all_performances(bd_dims, path, conversion_func,from_fitfile)
    return np.mean(all_performances)/max_performance


def coverages(bd_shape,BD_directory, runs, archive_file_paths):
    stats = []
    if not isinstance(archive_file_paths,list):
        archive_file_paths=[archive_file_paths]
    for path in archive_file_paths:
        for run in runs:
            stats.append(_coverage(bd_shape,BD_directory, run, path))
    print_conditional("global coverages"+str(stats))
    return stats

def _coverage(bd_shape,BD_directory, run, archive_file_path):
    """
    Measures how many cells of the feature space a run of an algorithm 
    is able to fill of the total number that are possible to fill.
    :return: 
    """

    path = get_archive_filepath(BD_directory, run, archive_file_path)
    all_non_empty_performances = get_bin_performances_uniquearchive(path,bd_shape[1])
    num_filled=len(all_non_empty_performances)
    max_num_filled=bd_shape[0]
    return float(num_filled)/float(max_num_filled)

def absolutecoverages(bd_shape,BD_directory, runs, archive_file_paths):
    stats = []
    if not isinstance(archive_file_paths,list):
        archive_file_paths=[archive_file_paths]
    for path in archive_file_paths:
        for run in runs:
            stats.append(_absolutecoverage(bd_shape,BD_directory, run, path))
    print_conditional("global coverages"+str(stats))
    return stats

def _absolutecoverage(bd_shape,BD_directory, run, archive_file_path):
    """
    Measures how many cells of the feature space a run of an algorithm
    is able to fill
    :return:
    """

    path = get_archive_filepath(BD_directory, run, archive_file_path)
    all_non_empty_performances = get_bin_performances_uniquearchive(path,bd_shape[1])
    num_filled=len(all_non_empty_performances)
    return float(num_filled)

# def _spread(bd_shape,BD_directory, run, archive_file_path,
#             individuals=[],distance_metric=norm_Euclidian_dist,
#             bd_start=1,comp=[]):
#     """
#     Measures how many cells of the feature space a run of an algorithm
#     is able to fill
#     :return:
#     """

#     path = get_archive_filepath(BD_directory, run, archive_file_path)
#     bd_list  = get_individual_bds(path,individuals,bd_start)
#     temp=0.0
#     comps=0.0




#     if not comp:
#         for i,bd1 in enumerate(bd_list):
#             for j,bd2 in enumerate(bd_list):
#                 if i!=j:
#                     temp+=distance_metric(bd1,bd2)
#                     comps+=1.0
#         temp/=comps
#         assert comps==len(bd_list)*(len(bd_list)-1)
#         return temp
#     else:
#         comp =  get_individual_bds(path,comp,bd_start)[0]


#         for i,bd1 in enumerate(bd_list):
#             temp+=distance_metric(bd1,comp)  # no need to check for equality since if comp==bd1 this should be penalised
#             comps+=1.0
#         temp/=comps
#         assert comps==len(bd_list)
#         return temp

# def uniqueness(BD_directory,runs, gener,targets, bin_indexes):
#     """
#     calculates uniqueness values, for combined archive and single archive
#     :param BD_directory:
#     :param runs:
#     :param archive_file_path:
#     :param bin_indexes
#     :return:
#     """
#     ub_dict={}
#     comb_ub_dict={}
#     for target in targets:
#         comb_unique_bins=set([])
#         unique_bins=[]
#         for run in runs:

#             u=set(parse_bins(BD_directory+"/results"+str(run)+"/analysis"+str(gener)+"_"+target+"REDUCED.dat",bin_indexes[target]))
#             unique_bins.append(u)
#             comb_unique_bins = comb_unique_bins | u
#         ub_dict[target]=unique_bins
#         comb_ub_dict[target]=comb_unique_bins

#     return ub_dict,comb_ub_dict






def add_boxplotlike_data(stats, y_bottom,y_mid,y_top, y_label,method_index,statistic="mean_SD"):
    """
    return x.25,x.50 and x.75
    :return:
    """
    if statistic=="median_IQR":
        x1,x2,x3=tuple(np.quantile(stats, q=[.25, .50, .75]))
    elif statistic=="mean_SD":
        sd = np.std(stats)
        x2 = np.mean(stats)
        x1 = x2 - sd
        x3 = x2 + sd
    elif statistic=="mean_SE":
        sd = np.std(stats)/np.sqrt(len(stats))
        x2 = np.mean(stats)
        x1 = x2 - sd
        x3 = x2 + sd
    elif statistic=="meanall_replicatesd": # when joining different fitfuns

        x2=np.mean(np.array(stats))
        sds=[np.std(stats[i]) for i in range(len(stats))]
        sd=np.mean(sds)
        x1= x2 - sd
        x3 = x2 + sd
        # assumes fitfun is first dimension of stats

    else:
        raise Exception("statistic %s not known"%(statistic))

    y_bottom[y_label][method_index].append(x1)
    y_mid[y_label][method_index].append(x2)
    y_top[y_label][method_index].append(x3)
# def translated_coverages(t,BD_dir,runs, targets):
#     d={target:[] for target in targets}
#     for run in runs:
#         for target, shape in targets.items():
#             archive_file = "analysis" + str(t) + "_" + target + "REDUCED.dat"
#             cov = _absolutecoverage(shape, BD_dir, run, archive_file)
#             d[target].append(cov)
#     print_conditional("translated coverages " + str(d))
#     return d

# def translated_spreads(t,BD_dir,runs,targets,bd_start,dists,individuals,comp):
#     d = {target: [] for target in targets}
#     for run in runs:
#         for target, shape in targets.items():
#             if individuals[run-1]:  # look for the unreduced archive
#                 archive_file = "analysis" + str(t) + "_" + target + ".dat"
#             else: # look for the reduced archive individuals
#                 archive_file = "analysis" + str(t) + "_" + target + "REDUCED.dat"
#             s = _spread(shape, BD_dir, run, archive_file,bd_start=bd_start.get(target,1),
#                         distance_metric=dists[target],
#                         individuals=individuals[run-1],
#                         comp=comp[run-1])
#             d[target].append(s)
#     print_conditional("translated spreads " + str(d))
#     return d

# def add_boxplotlike_data(stats, y_bottom,y_mid,y_top, y_label,method_index,statistic="mean_SD"):
#     """
#     return x.25,x.50 and x.75
#     :return:
#     """
#     if statistic=="median_IQR":
#         x1,x2,x3=tuple(np.quantile(stats, q=[.25, .50, .75]))
#     elif statistic=="mean_SD":
#         sd = np.std(stats)
#         x2 = np.mean(stats)
#         x1 = x2 - sd
#         x3 = x2 + sd
#     elif statistic=="meanall_replicatesd": # when joining different fitfuns

#         x2=np.mean(np.array(stats))
#         sds=[np.std(stats[i]) for i in range(len(stats))]
#         sd=np.mean(sds)
#         x1= x2 - sd
#         x3 = x2 + sd
#         # assumes fitfun is first dimension of stats

#     else:
#         raise Exception("statistic %s not known"%(statistic))

#     y_bottom[y_label][method_index].append(x1)
#     y_mid[y_label][method_index].append(x2)
#     y_top[y_label][method_index].append(x3)

# def convert_CoverageFitness(fitness,grid_size=0.1212,max_velocity=0.10, time_per_trial=120, total_cells=1090):
#     """
#     :param fitness: the coverage fitness e.g. 0.10 means 10% of all cells are visited
#     :param grid_size: e.g. each cell 0.14 m
#     :param max_velocity: e.g. maximal velocity of thymio is 0.10m/s
#     :param time_per_trial: e.g. 120 seconds
#     :param arena_size: e.g. (4,4) is a 4-by-4 arena
#     :return:
#     """
#     max_cells_per_second = max_velocity/grid_size
#     max_cells_per_trial = np.ceil(max_cells_per_second*time_per_trial)

#     visited_cells = np.ceil(total_cells*fitness)  #e.g. 160 cells, then fitness=0.1  means 16 cells visited
#     return visited_cells/max_cells_per_trial  # coverage now means visited cells compared to the maximum possible

# def print_best_individuals(BD_dir,outfile, number,generation):
#     solutions, indexes = get_best_individuals(BD_dir, range(1,6), "archive_"+str(generation)+".dat",number,criterion="fitness")
#     with open(outfile+"fitness.txt", 'w') as f:
#         i=0
#         for key,value in solutions.items():
#             f.write("%s %s %.3f \n "%(indexes[i], str(key),value[0]))
#             i+=1
#         f.flush()



#     solutions, indexes = get_best_individuals(BD_dir, range(1, 6), "archive_"+str(generation)+".dat", number, criterion="diversity")
#     with open(outfile + "diversity.txt", 'w') as f:
#         i = 0
#         for array in solutions:
#             f.write("%s %s %.3f \n" % (indexes[i], array[0:-1], array[-1]))
#             i += 1


def try_add_performance_data(i,bd_shapes,bybin_list,directory,runs,archive_files, y_bottom,y_mid,y_top,from_fitfile=False):

    bd_dims=bd_shapes[i][1]

    try:
        avg_perform = avg_performances(bd_dims,  directory, runs, archive_files, 1.0,
                                       conversion_func=None,from_fitfile=from_fitfile)
        add_boxplotlike_data(avg_perform, y_bottom, y_mid, y_top, y_label="Average_fitness", method_index=i,statistic="mean_SE")

        global_perform = global_performances(bd_dims, directory, runs, archive_files, 1.0,
                                             conversion_func=None,from_fitfile=from_fitfile)
        add_boxplotlike_data(global_perform, y_bottom, y_mid, y_top, y_label="Global_fitness", method_index=i,statistic="mean_SE")

        if not from_fitfile:
            # coverage = coverages(bd_shapes[i], directory, runs, archive_file)
            # add_boxplotlike_data(coverage, y_bottom, y_mid, y_top, y_label="coverage", method_index=i)

            absolutecoverage = absolutecoverages(bd_shapes[i], directory,  runs, archive_files)
            add_boxplotlike_data(absolutecoverage, y_bottom, y_mid, y_top, y_label="Number_of_solutions", method_index=i,statistic="mean_SE")

    except IOError as e:
            print(e)

def get_time(t,type):
    if "meta" in type:
        time=int(t//20)
    else:
        time=t
    return time

# def coverage_development_plots(title,runs,times,BD_directory,title_tag, bd_type, legend_labels,bybin_list,fig=None,ax=None,metrics=None):
#
#     colors=["C0","C1","C2","C3","C4"]  # colors for the lines
#     # (numsides, style, angle)
#     markers=["o","^","s","X","D"] # markers for the lines
#     y_labels=["Map coverage"]
#
#
#     boxes=[(.10,.40),(.10,.60),(.10,.60),(.45,.15),(0.20,0.20),(0.20,0.20)] # where to place the legend box
#     y_bottom={ylabel:[[] for i in bd_type] for ylabel in y_labels}
#     y_mid = {ylabel: [[] for i in bd_type]  for ylabel in y_labels}
#     y_top = {ylabel: [[] for i in bd_type]  for ylabel in y_labels}
#     max_gen_dict={}
#     for i, bd in enumerate(bd_type):
#         print(legend_labels[i])
#         if bd.endswith("meta"):
#             max_gen = get_max_metagen(bd)
#             abs_coverages = []
#             for t in range(0,max_gen,100)
#
#             try:
#                 time=get_time(t,bd_type[i])
#
#                     default = max_gen_dict["default"]
#                     M = max_gen_dict.get(bd_type[i], default)
#                     if time <= M:
#                         archive_files=[]
#                         for index in range(LAMBDA):
#                             archive_file, directory = get_archive_plus_dir(BD_directory,bd_type[i],time,index)
#                             archive_files.append(archive_file)
#                         abs_coverage=absolutecoverages(bd_shapes[i], directory, runs, archive_files)
#                         abs_coverages.append(abs_coverage)
#                 else:
#                     archive_file, directory = get_archive_plus_dir(BD_directory,bd_type[i],time)
#                     abs_coverage=absolutecoverages(bd_shapes[i], directory, runs, archive_file)
#                     abs_coverages.append(abs_coverage)
#
#             except Exception as e:
#                 print(e)
#             add_boxplotlike_data(abs_coverages, y_bottom, y_mid, y_top, y_label="Map coverage",method_index=i,
#                                  statistic="meanall_replicatesd")
#
#
#     j=0
#     maximum_line = (times,[4096 for i in times])
#     annots = {"text": "maximal coverage=4096","xy":(5000,4400),"xytext":(5000,4400),
#               "fontsize":22,"align": "center"}
#     for label in y_labels:
#         #ylim=[0,4500]
#
#         createPlot(y_mid[label],x_values=np.array(times),
#                    save_filename=BD_directory + "/" + label + ".pdf", legend_labels=legend_labels,
#                    colors=colors,markers=markers,xlabel="Generations",ylabel=label.replace("_"," "),
#                    xlim=[0,times[-1]+500],xscale="linear",yscale="log",ylim=[10**0.0,10**4],
#                    legendbox=[1.005,0.36],annotations=[annots],xticks=[],yticks=[],task_markers=[],scatter=False,
#                    legend_cols=1,legend_fontsize=26,legend_indexes=[],additional_lines=[maximum_line],index_x=[],
#                    xaxis_style="plain",y_err=[],force=True,fill_between=(y_bottom[label],y_top[label]),
#                    ax=ax,title=title )
#
#         j+=1




def development_plots(title,runs,BD_directory,bd_type, legend_labels,bybin_list,fig=None,ax=None,metrics=None):

    # bd_type = ["history","cvt_mutualinfo","cvt_mutualinfoact","cvt_spirit"]  #legend label
    #
    # legend_labels=["handcrafted","mutualinfo","mutualinfoact","spirit"]  # labels for the legend
    # colors=["C"+str(i) for i in range(len(bd_type))]  # colors for the lines
    # # (numsides, style, angle)
    # markers=[(3,1,0), (3,2,0),(4,1,0),(4,2,0)] # markers for the lines
    # bd_shapes = [32**2, 1000,1000,1000]  # shape of the characterisation
    # y_labels=["global_performance","global_reliability","precision","coverage"]

    # bd_type = ["baseline","history","cvt_rab_spirit","Gomes_sdbc_walls_and_robots_std","environment_diversity","environment_diversity"]  #legend label
    # legend_labels=["design","handcrafted","SPIRIT","SDBC","QED","QED-Translated"]  # labels for the legend




    colors=["C0","C1","C2","C3","C4","C5","C6","C7","C8"]  # colors for the lines
    # (numsides, style, angle)
    markers=["o","^","s","X","D",r"$\dagger$","*",r"$\star$",'O'] # markers for the lines
    y_labels=["Global_fitness","Average_fitness","Number_of_solutions"]


    boxes=[(.10,.40),(.10,.60),(.10,.60),(.45,.15),(0.20,0.20),(0.20,0.20)] # where to place the legend box
    y_bottom={ylabel:[[] for i in bd_type] for ylabel in y_labels}
    y_mid = {ylabel: [[] for i in bd_type]  for ylabel in y_labels}
    y_top = {ylabel: [[] for i in bd_type]  for ylabel in y_labels}

    if args.DEST.endswith("RASTRI"):
        bins_ME = 25
        bins_meta = 25
    else:
        bins_ME=10
        bins_meta=10

    function_eval_times=[]
    for i, bd in enumerate(bd_type):

        print(legend_labels[i])
        bd, npop, step = bd
        if "meta" in bd:
            function_eval_times.append(np.array(np.linspace(0, MAX_FUN_EVALS, bins_meta + 1)))
            bins = bins_meta + 1
            max_gens=[]
            for r in runs:
                filename = get_output_folder_test(folder=args.DEST, condition=bd, replicate=r)
                max_gens.append(check_metagens_forfuneval(filename,
                                          n_pop=npop, fun_evals=MAX_FUN_EVALS))
            timebins = [get_timebins(BD_directory,bd,max_gen=max_gen,bins=bins,step=step) for max_gen in max_gens]
            archive_files = []
            for t in range(len(function_eval_times[-1])):
                    print(t,"/",len(function_eval_times[-1]))
                    for r, run in enumerate(runs):
                        time = timebins[r][t]
                        for index in range(npop):
                            archive_file, directory = get_archive_plus_dir(BD_directory, bd, time, index)
                            archive_files.append(directory+str(run)+"/"+archive_file)
                    try_add_performance_data(i,bd_shapes,bybin_list,"",[""],archive_files, y_bottom,y_mid,y_top,from_fitfile=False)
        else:
            function_eval_times.append(np.array(np.linspace(0, MAX_FUN_EVALS, bins_ME + 1)))
            bins=bins_ME
            end=MAX_FUN_EVALS//BATCH_SIZE
            delta = end // bins_ME
            times = range(0, end+delta , delta)
            for time in times:
                print(time,"/",times[-1])
                archive_file, directory = get_archive_plus_dir(BD_directory,bd,time)
                try_add_performance_data(i,bd_shapes,bybin_list,directory,runs,archive_file, y_bottom,y_mid,y_top,from_fitfile=False)



    j=0

    for label in y_labels:
        scale="log" if label == "Number_of_solutions" else "linear"
        axis = None #if ax is None else ax[j]
        temp_labels = copy.copy(legend_labels)
        print("separate")
        y_bottom[label]=[np.array(method)  for method in y_bottom[label]]
        y_top[label] = [np.array(method) for method in y_top[label]]
        createPlot(y_mid[label],x_values=function_eval_times,colors=colors,markers=markers,xlabel="Function evaluations",ylabel=label.replace("_"," "),ylim=None,
                   save_filename=args.DEST+"/"+title+"_"+label+".pdf",legend_labels=temp_labels,
                   xlim=[0,MAX_FUN_EVALS*1.05],xscale="linear",yscale=scale,
               legendbox=None,annotations=[],xticks=[],yticks=[],task_markers=[],scatter=False,
               legend_cols=1,legend_fontsize=15,legend_indexes=[],additional_lines=[],index_x=[],
               xaxis_style="plain",y_err=[],force=True,fill_between=(y_bottom[label],y_top[label]),
                   ax=axis,title="",legend_setting="empty" )

        createPlot(y_mid[label],x_values=function_eval_times,colors=colors,markers=markers,xlabel="Function evaluations",ylabel=label.replace("_"," "),ylim=None,
                   save_filename=args.DEST+"/"+title+"_"+label+"_legend.pdf",legend_labels=temp_labels,
                   xlim=[0,MAX_FUN_EVALS*1.05],xscale="linear",yscale=scale,
               legendbox=None,annotations=[],xticks=[],yticks=[],task_markers=[],scatter=False,
               legend_cols=1,legend_fontsize=15,legend_indexes=[],additional_lines=[],index_x=[],
               xaxis_style="plain",y_err=[],force=True,fill_between=(y_bottom[label],y_top[label]),
                   ax=axis,title="",legend_setting="separate" )

        j+=1





def apply_star_and_bold(text,descriptor,target,max_descriptor,second_max_descriptor):
    if descriptor==max_descriptor:
        text=text+r"^{*}"
        if target == descriptor:
            text=r"$"+text+r"$"
        else:
            text=r"$\mathbf{"+text+"}$"
    else:
        if descriptor==second_max_descriptor:
            text = r"$\mathbf{" + text + "}$"
    return text





def create_all_development_plots(tag):

    bybin_list=["bd", "bd", "bd", "bd", "bd", "bd"]
    fig, axs = PLT.subplots(1, 4, figsize=(40, 10))  # coverage, avg perf., global perf., global reliability
    development_plots(title=tag,runs=runs,
                          BD_directory=args.DEST,bd_type=bd_type,
                          legend_labels=legend_labels,bybin_list=bybin_list,
                          ax = axs)

def create_coverage_development_plots():

    bybin_list=["bd", "individual", "individual", "bd", "bd", ""]
    times=range(0,7250, 250)
    fig, axs = PLT.subplots(1,1, figsize=(10, 10))  # coverage, avg perf., global perf., global reliability
    coverage_development_plots(title="",runs=runs, times=times,
                          BD_directory=args.DEST,
                         title_tag="",bd_type=bd_type,
                          legend_labels=legend_labels,bybin_list=bybin_list,
                          ax = axs)


    finish_fig(fig, args.DEST +"/coverage_plot.pdf")




def damage_performance(conditions, labels, max_fun_evals=None, title="",ylim=None,metric="Summed pairwise distance ($m$)",yscale="linear"):


    plotlines=[[] for i in conditions]
    plotstds = [[] for i in conditions]
    bottoms = [[] for i in conditions]
    tops = [[] for i in conditions]
    ts=[]
    final_metafitness=open("final_metafitness"+title+".txt","w")
    for c, condition in enumerate(conditions):

        condition, pop, step = condition
        filenames = [get_output_folder_test(folder=args.DEST, condition=condition, replicate=r) for r in runs]
        max_len=0
        y_bins=[]
        final_values = []
        for run, f in enumerate(filenames):
            all_values = []

            y_bins.append([])
            t = 0
            maxmetagen = check_metagens_forfuneval(f,
                                      n_pop=pop, fun_evals=max_fun_evals)
            while t<=maxmetagen:
                path = f + "/recovered_perf"+str(t)+".dat"
                try:
                    line = open(path).readline()
                    values = line.split("\t")
                    all_values.append(np.mean([float(values[i]) for i in range(pop)])) # convert to float
                except:
                    print(f + "/recovered_perf"+str(t)+".dat not found")
                    break
                t+=step
            # now divide data in ten as closely as possible
            delta=(t - step)/10
            metagenerations=0
            x_candidates=range(0,t,step)

            if BY_FUNEVAL:
                N_bins=26
                y_bins[-1].append(all_values[0])
                for i in range(1,N_bins):
                    metagenerations += delta
                    closest = int(round(metagenerations/step)) #
                    print("closest ",closest)
                    y_bins[-1].append(all_values[closest])
                    print("y = ", all_values[closest])
                startfinal = metagenerations - delta
                closest = int(round(startfinal / step))  #
                final_values.append(np.mean(all_values[closest:]))
            else:
                for i in range(len(all_values)):
                    y_bins[-1].append(np.array(all_values[i]))
                if run==0:
                    minimax_len=len(all_values)
                else:
                    if len(all_values) < minimax_len:
                        minimax_len = len(all_values)


        assert len(y_bins) == len(runs)
        y_bins = np.array(y_bins)
        if BY_FUNEVAL:

            M = np.mean(y_bins,axis=0)

            S = np.std(y_bins,axis=0)/ np.sqrt(len(temp))
        else:
            M=[]
            S=[]
            for i in range(minimax_len):
                temp = []
                for r,_r in enumerate(runs):
                    temp.append(y_bins[r][i])
                if RUNNING_AVERAGE and M:
                    rmean= 0.75*M[-1] + 0.25*np.mean(temp)
                    M.append(rmean)
                    S.append(np.std(temp) / np.sqrt(len(temp)))
                else:
                    M.append(np.mean(temp))
                    S.append(np.std(temp)/np.sqrt(len(temp)))
            M=np.array(M)
            S=np.array(S)
            ts.append(np.linspace(0,minimax_len*step,minimax_len +1))



        print("M=",M)
        print("S=",S)
        plotlines[c]=M
        bottoms[c]=M - S
        tops[c]=M + S
        t+=step
        #m=np.mean(final_values)
        #s=np.std(final_values)
        #final_metafitness.write(labels[c] + r" & $%.1f \pm %.1f$ "%(m,s) + "\n" )

    if BY_FUNEVAL:
        ts = np.linspace(0, MAX_FUN_EVALS, N_bins)
        xlim = [0, MAX_FUN_EVALS * 1.05]
    else:
        maxgen=minimax_len*step
        xlim = None

    markers = [(2, 1, 0), (3, 1, 0), (5, 1, 0), (3, 2, 0),  (4, 1, 0), (4, 2, 0),
                           (4, 3, 0),(5, 1, 0), (5, 2, 0),
                           (5, 3, 0)]
    xlab = "Function evaluations" if BY_FUNEVAL else "Meta-generations"

    createPlot(plotlines, x_values=ts, colors=["C"+str(i) for i in range(len(plotlines))], xlabel=xlab,
                   ylabel=metric, ylim=ylim,markers=markers,
                   save_filename=args.DEST + "/"+title+"recovery.pdf", legend_labels=labels,
                   xlim=xlim, xscale="linear", yscale=yscale,legendbox=None, fill_between=[bottoms,tops],force=True,
                   legend_setting="empty",legend_fontsize=26)

    createPlot(plotlines, x_values=ts, colors=["C"+str(i) for i in range(len(plotlines))], xlabel=xlab,
                   ylabel=metric, ylim=ylim,markers=markers,
                   save_filename=args.DEST + "/"+title+"recovery_legend.pdf", legend_labels=labels,
                   xlim=xlim, xscale="linear", yscale=yscale,legendbox=None, fill_between=[bottoms,tops],force=True,
                   legend_setting="separate",legend_fontsize=15)


def parameter_logs(type, c,ylabel, ylim=None,filename=None):
    n_pop=11
    if type=="epochs" or type=="meta-fitness":
        iterations_dict = {
                           "featuresets_epochrl_b2p10": "_featuresets",
        "epochrl_b2p10": ""}
    else:
        raise Exception("unknown parameter log type")
    max_t = MAX_FUN_EVALS

    x=[(i+1)*MAX_FUN_EVALS//100 for i in range(101)]
    if type=="meta-fitness": # not realy a parameter but also run-specific
        for suffix, leg in iterations_dict.items():
            condition=c+suffix
            y_bins=[]
            pop=10
            filenames = [get_output_folder_test(folder=args.DEST, condition=condition, replicate=r) for r in runs]
            for run, f in enumerate(filenames):

                all_values = []

                y_bins.append([])
                t = 0
                maxmetagen = check_metagens_forfuneval(f,
                                          n_pop=pop, fun_evals=MAX_FUN_EVALS)
                step=20
                while t<=maxmetagen:
                    path = f + "/recovered_perf"+str(t)+".dat"
                    try:
                        line = open(path).readline()
                        values = line.split("\t")
                        all_values.append(np.mean([float(values[i]) for i in range(pop)])) # convert to float
                    except:
                        print(f + "/recovered_perf"+str(t)+".dat not found")
                        break
                    t+=step
                # now divide data in ten as closely as possible
                N_bins = 100
                y_bins[-1].append(all_values[0])
                delta = maxmetagen/N_bins
                metagenerations=0
                for i in range(1, N_bins+1):
                    metagenerations += delta
                    closest = int(round(metagenerations / step))  #
                    print("closest ", closest)
                    y_bins[-1].append(all_values[closest])
                    print("y = ", all_values[closest])
            y_means = y_bins
            markers = ["X", "D", "o", "x", "^"]
            xlab = "Function evaluations"
            createPlot(y_bins, x_values=np.array(x), colors=["grey"] * len(runs), xlabel=xlab,
                       ylabel=ylabel, ylim=ylim, markers=markers,
                       save_filename=args.DEST + "/" + filename + leg + ".pdf",
                       legend_labels=["replicate "+str(r+1) for r, run in enumerate(runs)],
                       xlim=[0, max_t * 1.05], xscale="linear", yscale="linear", legendbox=None, force=True,
                       legend_setting="default")
    else:
        for suffix, leg in iterations_dict.items():
                y = [[[] for i in range(101)] for r in runs]
                #infix = "_added1genes_" if "endogeneous" in suffix else "_added0genes_"
                condition=c+suffix
                filenames = [get_output_folder_test(folder=args.DEST, condition=condition, replicate=r) for r in runs]
                failures=0
                for r, f in enumerate(filenames):
                    path = f + "/" + type + "_log.txt"
                    try:
                        lines = read_spacedelimited(path)
                        max_eval=0
                        for n, line in enumerate(lines):

                            if n % n_pop == 1:
                                xx = float(line[1])
                                yy = float(line[0])
                                if xx > max_eval:
                                    for i, bin in enumerate(x):
                                        if xx < bin:
                                            y[r][i].append(yy)
                            else:
                                continue


                    except:
                        print(path + " not found")
                        failures+=1
                if failures == len(filenames): # none of the runs finished this far
                    raise Exception("no runs finished for ", filenames)

                y_means = [[np.mean(y[r][i]) for i in range(101)] for r in range(len(runs))]
                y_sd = [[np.std(y[r][i])/np.sqrt(len(y[r][i])) for i in range(101)] for r in range(len(runs))]
                y_bottom = [np.array(y_means[r]) - np.array(y_sd[r]) for r in range(len(runs))]
                y_top = [np.array(y_means[r]) + np.array(y_sd[r]) for r in range(len(runs))]
                markers = ["X","D","o","x","^"]
                xlab = "Function evaluations"
                x_ref=np.array(x)
                y_ref = np.zeros(101) + 5
                createPlot(y_means, x_values=np.array(x), colors=["grey"]*len(runs), xlabel=xlab,
                                   ylabel=ylabel, ylim=ylim,markers=markers,
                                   save_filename=args.DEST + "/"+filename+leg+".pdf", legend_labels=["replicate "+str(r+1) for r, run in enumerate(runs)],
                                   xlim=[0, max_t*1.05], xscale="linear", yscale="linear",legendbox=None,force=True,
                                   legend_setting="default",fill_between=(np.array(y_bottom),np.array(y_top)),additional_lines=[(x_ref,y_ref)])

# def damage_performance_sorted(condition, max_gen, step):
#     filenames = [get_output_folder_test(folder=args.DEST, condition=condition, replicate=r) for r in runs]
#     pop = 5
#     meanlines = []
#     replicate_values = [[] for i in range(len(runs))]
#     bottom=[]
#     top=[]
#     ts = range(0, max_gen + step, step)
#     for t in ts:
#         repls=[]
#         all_values=[]
#         for j, f in enumerate(filenames):
#             path = f + "/recovered_perf" + str(t) + ".dat"
#             line = open(path).readline()
#             values = line.split("\t")
#             temp_values=[]
#             for i in range(pop):
#                 temp_values.append(float(values[i]))
#             all_values.append(np.mean(temp_values))  #get population mean
#             repls.append(all_values[-1])
#             replicate_values[j].append(repls)  # append it
#         m=np.mean(repls)
#         meanlines.append(m)
#         sd = np.std(repls)
#         #     temp_values=np.sort(temp_values)[::-1]
#         #     for i in range(pop):
#         #         all_values[i].append(temp_values[i])
#         # for i in range(pop):
#         #     m=np.mean(all_values[i])
#         #     plotlines[i].append(m)
#         #     if i == 0:
#         #         sd= np.std(all_values[i])
#         #     else:
#         #         sd=0
#         bottom.append(m - sd)
#         top.append(m + sd)
#     fill_between=([np.array(bottom)],[np.array(top)])
#     markers = [(1, 1, 0), (1, 2, 0), (1, 3, 0), (3, 1, 0), (3, 2, 0), (3, 3, 0), (4, 1, 0), (4, 2, 0),
#                (4, 3, 0)]
#     y_vals=[meanlines] #+ replicate_values
#     createPlot(y_vals, x_values=np.array(ts), colors=["C" + str(i) for i in range(6)],
#                xlabel="Meta-generations",ylim=None,
#                ylabel="Damage recovery ($m$)",markers=markers,line_width=[5],ms=[16],
#                save_filename=args.DEST + "/Results/recovery_m.pdf", legend_labels=None,
#                xlim=[0, ts[-1] + 10], xscale="linear", yscale="linear",fill_between=fill_between)
# def damage_performance_MEANSD(condition, max_gen, step):
#     filenames=[get_output_folder_test(folder=args.DEST,condition=condition,replicate=r) for r in runs]
#     \lambda=5
#     plotlines=[]
#     bottom=[]
#     top=[]
#     ts = range(0, max_gen + step, step)
#     for t in ts:
#         replicate_vals=[]
#         for f in filenames:
#             path = f + "/recovered_perf"+str(t)+".dat"
#             line = open(path).readline()
#             values = line.split("\t")
#             float_values=[]
#             for i in range(pop):
#                 float_values.append(float(values[i]))
#             max_value = np.max(float_values)
#             replicate_vals.append(max_value)
#         m = np.mean(replicate_vals)
#         sd = np.std(replicate_vals)
#         l = m - sd
#         u = m + sd
#         plotlines.append(m)
#         bottom.append(l)
#         top.append(u)
#
#     markers = [(1, 1, 0), (1, 2, 0), (1, 3, 0), (3, 1, 0), (3, 2, 0), (3, 3, 0), (4, 1, 0), (4, 2, 0),
#                            (4, 3, 0)]
#     createPlot([plotlines], x_values=np.array(ts), colors=["C0"], xlabel="Meta-generations",
#                ylabel="Damage recovery", ylim=None,markers=markers,
#                save_filename=args.DEST + "/Results/recoveryMEANSD.pdf", legend_labels=None,#legend_labels=["i"+str(i) for i in range(pop)],
#                xlim=[0, ts[-1] + 10], xscale="linear", yscale="linear",fill_between=(bottom,top))

def epochs(metric):
    legend_dict = {"linfm":"Linear", "nonlinfm":"NonLinear", "selectionfm":"Selection"}
    iterations_dict = {"b1p1": "5 generations","b2p1": "10 generations", "b5p1": "25 generations",
                       "b10p1":"50 generations",
                      "epochannealing_b10p1": "Annealing generations", "epochrl_b10p1":"RL generations", "epochendogeneous_b10p1": "Endogenous generations"}
    ylim=[0,50000]
    for fm in ["linfm","nonlinfm","selectionfm"]:
        ld = legend_dict[fm]
        print("FM =", ld)
        bds = []
        legend = []
        for suffix, leg in iterations_dict.items():
            num_added = "1" if "endogeneous" in suffix else "0"
            bd = "damage_meta_"+fm + "_added"+num_added+"genes_" + suffix
            bds.append(bd) # file system label for bd
            legend.append( leg)
        # bds.append("random_"+fm+"_")
        # legend.append("Random feature-map")
        damage_performance(conditions=bds, labels=legend, max_fun_evals=MAX_FUN_EVALS,title=ld+"EPOCHS",ylim=ylim,metric=metric)


def featuremap_nodamagemetafitness(metric):

    legend_dict = {"linfm":"Linear", "nonlinfm":"Non-linear", "selectionfm":"Feature-selector"}

    ylim=[0,50000]
    bds = []
    legend = []
    for fm, leg in legend_dict.items():
        print("FM =", leg)
        bd = "nodamage_meta_"+fm + "_added0genes_b1p1"
        bds.append(bd) # file system label for bd
        legend.append( leg)
        # bds.append("random_"+fm+"_")
        # legend.append("Random feature-map")
    damage_performance(conditions=bds, labels=legend, step=200, max_fun_evals=MAX_FUN_EVALS,title="nodamageFM",ylim=ylim,metric=metric)


def featuremap_metafitness(metric):

    legend_dict = OrderedDict({"nonlinfm":"Non-linear","selectionfm":"Feature-selector","linfm":"Linear" })

    ylim=[0,25000]
    bds = []
    legend = []
    for fm, leg in legend_dict.items():
        print("FM =", leg)
        bd = "damage_meta_"+fm + "_added0genes_b1p1"
        bds.append(bd) # file system label for bd
        legend.append( leg)
        # bds.append("random_"+fm+"_")
        # legend.append("Random feature-map")
    damage_performance(conditions=bds, labels=legend, step=200, max_fun_evals=100 * 10 ** 6,title="FM",ylim=ylim,metric=metric)

def compare_metafitness(bds,legend,metric,ylim,title,yscale="linear"):

    damage_performance(conditions=bds, labels=legend, max_fun_evals=MAX_FUN_EVALS,title=title,ylim=ylim,metric=metric,yscale=yscale)

def epochs_metafitness():

    legend_dict = {"linfm":"Linear", "nonlinfm":"NonLinear", "selectionfm":"Selection"}
    iterations_dict = {"b1p1": "5 generations","b2p1": "10 generations", "b5p1": "25 generations",
                       "b10p1":"50 generations",
                      "epochannealing_b10p1": "Annealing generations", "epochrl_b10p1":"RL generations",
                       "epochendogeneous_b10p1": "Endogenous generations"}
    ylim_dict={"linfm":[0,2500],"nonlinfm":[0,20000],"selectionfm":[0,6500]}
    for fm in ["linfm","nonlinfm","selectionfm"]:
        ld = legend_dict[fm]
        ylim=ylim_dict[fm]
        print("FM =", ld)
        bds = []
        legend = []
        for suffix, leg in iterations_dict.items():
            num_added = "1" if "endogeneous" in suffix else "0"
            bd = "damage_meta_"+fm + "_added"+num_added+"genes_" + suffix
            bds.append(bd) # file system label for bd
            legend.append( leg)
        # bds.append("random_"+fm+"_")
        # legend.append("Random feature-map")
        damage_performance(conditions=bds, labels=legend, step=200, max_fun_evals=MAX_FUN_EVALS,title=ld+"EPOCHS",ylim=ylim)


def mutation_rates_metafitness():

    legend_dict = {"linfm":"Linear", "nonlinfm":"NonLinear", "selectionfm":"Selection"}
    iterations_dict = {"b1p1": "Mutation rate 0.125","b1p1m2": "Mutation rate 0.25",
                       "b1p1m4": "Mutation rate 0.50","b1p1m8": "Mutation rate 1.0",
                       "mutationannealing_b1p1m8": "Annealing mutation rate",
                       "mutationrl_b1p1m8":"RL mutation rate",
                       "mutationendogeneous_b1p1m8": "Endogenous mutation rate"}
    ylim_dict={"linfm":[0,2500],"nonlinfm":[0,20000],"selectionfm":[0,6500]}
    for fm in ["linfm","nonlinfm","selectionfm"]:
        ld = legend_dict[fm]
        ylim=ylim_dict[fm]
        print("FM =", ld)
        bds = []
        legend = []
        for suffix, leg in iterations_dict.items():
            num_added = "1" if "endogeneous" in suffix else "0"
            bd = "damage_meta_" + fm + "_added" + num_added + "genes_" + suffix
            bds.append(bd) # file system label for bd
            legend.append( leg)
        # bds.append("random_" + fm + "_")
        # legend.append("Random feature-map")
        damage_performance(conditions=bds, labels=legend, step=200, max_fun_evals=100 * 10 ** 6,title=ld+"MR",ylim=ylim)



# def epochs():
#     legend_dict = {"linfm": "LinearFM", "nonlinfm": "NonLinearFM", "selectionfm": "Selection"}
#     iterations_dict = {"b1p1": "5g", "b2p1": "10g", "b5p1": "25g", "b10p1": "50g"}
#     ylim=[0,50000]
#
#     for fm in ["linfm","nonlinfm","selectionfm"]:
#         bds = []
#         legend = []
#         ld = legend_dict[fm]
#         for suffix in ["b1p1","b2p1","b5p1"]:
#             bd = "damage_meta_"+fm + "_added0genes_" + suffix
#             bds.append(bd) # file system label for bd
#             legend.append( ld + " " + iterations_dict[suffix])
#             damage_performance(conditions=bds, labels=legend, step=50, max_fun_evals=100 * 10 ** 6,ylim=ylim)
        #bds.append("damage_meta_" + fm + "_added0genes_epochannealingb2p1")  # file system label for bd
        #legend.append(ld + " Annealing")
        #bds.append("damage_meta_" + fm + "_added0genes_epochendogeneousb10p1")  # file system label for bd
        #legend.append(ld + " Endogenous")




if __name__ == "__main__":


    if args.DEST.endswith("RHEX"):
        # RHEX
        # global
        runs=[str(i) for i in range(2,6)]
        fitfuns = [""]
        parameter_logs("meta-fitness","damage_meta_",r"Meta-fitness (distance in $m$)",filename="Damage_parameter_control_metafitness")
        parameter_logs("meta-fitness", "envir_meta_", r"Meta-fitness (distance in $m$)",filename="Obstacle_parameter_control_metafitness")
        parameter_logs("epochs","damage_meta_","Generations per meta-generation",filename="Damage_parameter_control")
        parameter_logs("epochs", "envir_meta_", "Generations per meta-generation",filename="Obstacle_parameter_control")

        for percentage_factor in ["10"]:
            n_pop=10
            step=20
            # development meta + RL vs control
            legend_labels = ["QD-Meta Damage Feature-sets", "QD-Meta Damage Trajectory",
                             "QD-Meta Obstacle Feature-sets", "QD-Meta Obstacle Trajectory", "AURORA","CVT-ME"]
            bd_type = [("damage_meta_featuresets_epochrl_b2p" + percentage_factor, n_pop,step),
                       ("damage_meta_epochrl_b2p" + percentage_factor, n_pop,step),
                       ("envir_meta_featuresets_epochrl_b2p" + percentage_factor, n_pop,step),
                       ("envir_meta_epochrl_b2p" + percentage_factor, n_pop,step),("AURORA_MLP",None,None), ("cvt50D_tra", None, None)]
            bd_shapes = [(4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4),(4096, 50)]  # shape of the characterisation

            create_all_development_plots(tag="p"+percentage_factor+"_METAvsCTRL")
            # development meta + RL vs meta
            legend_labels = ["Damage Feature-sets", "Damage Feature-sets (static)",
                             "Damage Trajectory","Damage Trajectory (static)",
                             "Obstacle Feature-sets", "Obstacle Feature-sets (static)",
                             "Obstacle Trajectory", "Obstacle Trajectory (static)"]
            bd_type = [("damage_meta_featuresets_epochrl_b2p" + percentage_factor, n_pop,step),
                       ("damage_meta_featuresets_b1p" + percentage_factor, 10, 20),
                       ("damage_meta_epochrl_b2p" + percentage_factor, n_pop,step),
                       ("damage_meta_b1p" + percentage_factor, 10, 20),
                       ("envir_meta_featuresets_epochrl_b2p" + percentage_factor, n_pop,step),
                       ("envir_meta_featuresets_b1p" + percentage_factor, 10, 20),
                       ("envir_meta_epochrl_b2p" + percentage_factor, n_pop,step),
                       ("envir_meta_b1p" + percentage_factor, n_pop,step)]

            bd_shapes = [(4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4)]  # shape of the characterisation

            create_all_development_plots(tag="p"+percentage_factor+"_META")


            legend_labels_damage=["Damage Feature-sets","Damage Feature-sets (static)","Damage Trajectory","Damage Trajectory (static)"]
            legend_labels_obstacle=["Obstacle Feature-sets","Obstacle Feature-sets (static)","Obstacle Trajectory","Obstacle Trajectory (static)"]
            bd_type_damages = [("damage_meta_featuresets_epochrl_b2p"+percentage_factor, n_pop,step),("damage_meta_featuresets_b1p"+percentage_factor, n_pop,step),
                               ("damage_meta_epochrl_b2p"+percentage_factor, n_pop,step),("damage_meta_b1p"+percentage_factor, n_pop,step)]
            bd_type_obstacles = [("envir_meta_featuresets_epochrl_b2p" + percentage_factor, n_pop,step),
                               ("envir_meta_featuresets_b1p" + percentage_factor, n_pop,step),
                               ("envir_meta_epochrl_b2p" + percentage_factor, n_pop,step),
                               ("envir_meta_b1p" + percentage_factor, n_pop,step)]
            compare_metafitness(bd_type_damages,legend_labels_damage,metric=r"Distance ($m$)", ylim=[3.5,7],title="Damage_p"+percentage_factor)
            compare_metafitness(bd_type_obstacles, legend_labels_obstacle, metric=r"Distance ($m$)", ylim=[2,4.5],title="Obstacle_p"+percentage_factor)

    elif args.DEST.endswith("RASTRI"):
        #RASTRI
        # global
        # runs = [str(i) for i in range(1,6)]
        # fitfuns = [""]
        # #parameter_logs("epochs", "Epochs")
        #
        percentage_factor="1"
        # # development meta popsizes
        # legend_labels = [r"QD-Meta $\lambda=1$", r"QD-Meta $\lambda=2$",r"QD-Meta $\lambda=5$", r"QD-Meta $\lambda=10$",r"QD-Meta $\lambda=20$"]
        # bd_type = [ ("trans_pop1_meta_b1p" + percentage_factor, 1, 200),
        #             ("trans_pop2_meta_b1p" + percentage_factor, 2, 200),
        #             ("trans_pop5_meta_b1p" + percentage_factor, 5, 200),
        #         ("trans_meta_b1p" + percentage_factor, 10, 200),
        #             ("trans_pop20_meta_b1p" + percentage_factor, 20, 200)]
        # bd_shapes = [(10000, 2), (10000, 2), (10000, 2),(10000, 2), (10000, 2)]  # shape of the characterisation
        #
        # #create_all_development_plots(tag="p" + percentage_factor + "_META_POPSIZE")
        # compare_metafitness(bd_type, legend_labels, metric=r"Meta-fitness ($\mathcal{F}_t$)", ylim=[10**4, 10 ** 7],
        #                    yscale="log",
        #                    title="pop")
        runs = [str(i) for i in range(1, 21)]
        # development meta + RL vs control
        # legend_labels = ["QD-Meta Dimensionality Reduction", "QD-Meta Translation","AURORA", "CVT-ME"]
        # bd_type = [("dimension_meta_epochrl_b2p" + percentage_factor, 10, 200),
        #            ("trans_meta_epochrl_b2p" + percentage_factor, 10, 200),
        #            ("AURORA_MLP", None, None),
        #            ("cvt_gen", None, None)]
        # bd_shapes = [(10000, 2), (10000, 2), (10000, 2),
        #              (10000,20)]  # shape of the characterisation
        #
        # create_all_development_plots(tag="p" + percentage_factor + "_METAvsCTRL")
        # development meta + RL vs meta
        # legend_labels = ["QD-Meta Dimensionality Reduction", "QD-Meta Dimensionality Reduction (static)",
        #                  "QD-Meta Translation", "QD-Meta Translation (static)"]
        # bd_type = [("dimension_meta_epochrl_b2p" + percentage_factor, 10, 1000),
        #            ("dimension_meta_b1p" + percentage_factor, 10, 1000),
        #            ("trans_meta_epochrl_b2p" + percentage_factor, 10, 1000),
        #            ("trans_meta_b1p" + percentage_factor, 10, 1000)]
        #
        # bd_shapes = [(10000, 2), (10000, 2), (10000, 2), (10000, 2)]  # shape of the characterisation
        #
        # create_all_development_plots(tag="p" + percentage_factor + "_META")

        legend_labels_dim= ["QD-Meta Dimensionality Decrement", "QD-Meta Dimensionality Decrement (static)"]
        legend_labels_trans = ["QD-Meta Translation", "QD-Meta Translation (static)"]
        bd_type_dim = [("dimension_meta_epochrl_b2p" + percentage_factor, 10, 100),
                           ("dimension_meta_b1p" + percentage_factor, 10, 100)]
        bd_type_trans = [("trans_meta_epochrl_b2p" + percentage_factor, 10, 100),
                             ("trans_meta_b1p" + percentage_factor, 10, 100)]

        compare_metafitness(bd_type_dim, legend_labels_dim, metric=r"Archive-summed fitness", ylim=[10**4,10**7],yscale="log",
                            title="dim")
        compare_metafitness(bd_type_trans, legend_labels_trans, metric=r"Archive-summed fitness", ylim=[10**4,10**7],yscale="log",
                            title="trans" + percentage_factor)
    else:
        raise Exception("specify a destination directory with RHEX or RASTRI")
