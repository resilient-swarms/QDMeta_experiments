
from adapt_performance import *
from check_metagens import *

if args.DEST.endswith("RASTRI"):
    MAXFIT=2000 # used in recovered_performance.hpp --> subtract again
    test_types = ["test_trans_damage", "test_dimension_damage"]
    MAXFUNEVALS=100*10**6
    function_evaluations = 100 # for the behaviour search
else:
    MAXFIT=0
    test_types = ["test_max_damage_damage", "test_max_envir_damage"]
    function_evaluations = 50  # for the behaviour search

conditions =  conditions #["cmaescheck"] + conditions
labels = labels # ["Traditional EA (CMA-ES)"] + labels
damage_dict = {"test_dimension_damage": 20, "test_trans_damage": 120, "test_max_damage_damage": 12, "test_max_envir_damage": 4}

def get_damagemaxperformance_from_file_cmaescheck(fileprefix, function_evaluations, slicing_factor):
    num_damages = 12
    max_value = -float("infinity")
    temp = [[] for i in range(num_damages)]
    num_lines=function_evaluations/float(slicing_factor)
    for damage in range(num_damages):
        filename = fileprefix+"/damage"+str(damage)+"/bestfit.dat"
        print("will get performance at " + filename)
        with open(filename, 'r') as f:
            for line in f:
                if line:  # avoid blank lines
                    stripped = line.strip().split(' ')[-1] # get the second value, the best value of the current generation
                    current_value=float(stripped)
                    if current_value > max_value:
                        max_value = current_value
                    temp[damage].append(max_value)
                    if len(temp[damage]) == num_lines:
                        break
    return temp



def get_damagemaxperformance_from_file_pop(filename,n_pop, strip_weights=False,type="dimension"):
    print("will get performance at " + filename)
    num_damages=damage_dict[type]
    x=[]
    temp = []
    weights = []
    temp_w = []
    count = 0
    with open(filename, 'r') as f:
        for line in f:
            if line:  # avoid blank lines
                stripped = (line.strip())
                if strip_weights and len(weights) <= n_pop:  # collecting weights
                    if stripped == "END WEIGHTS":
                        weights.append(temp_w)
                        temp_w = []
                    elif stripped.startswith("END"):
                        continue
                    else:
                        temp_w.append(np.array(stripped.split(),dtype=float))
                elif stripped == "END STATS POPULATION MEAN":
                    continue
                else:  # collecting performance data
                    if stripped.startswith("DAMAGE") or stripped.startswith("ENVIR"): # start new damage
                        if count % num_damages == 0 :
                            x.append([])
                            x[-1].append([])
                        else:
                            x[-1].append([])
                        count+=1
                    elif stripped.startswith("END"):
                        continue
                    else:
                        x[-1][-1].append(float(stripped))

    return weights, x


def get_damageperformances_pop(mean_lines,sd_lines1, sd_lines2, n_pop, condition, test_type, replicates,strip_weights,
                               slicing_factor=1,selection_criterion="train_performance"):


    max_index=0

    num_damages=damage_dict[test_type]
    #if type!="test": assert(selection_criterion is None)
    funevs=int(function_evaluations/float(slicing_factor))
    lines=np.zeros((len(replicates),num_damages,funevs))
    for i, replicate in enumerate(replicates):
        if n_pop > 1:
            if selection_criterion == "train_performance":  # reliable choice as it takes into account database changes and 100% of solutions in map
                folder = get_output_folder_test(args.DEST,condition,replicate)
                if args.DEST.endswith("RASTRI"):
                    # stopped exactly at 100 mill --> get last
                    maxmetagen = get_maxmetagen_in_folder(folder)
                else:
                    maxmetagen = check_metagens_forfuneval(folder,
                                                           n_pop=n_pop, fun_evals=12*10**6)
                path = folder + "/recovered_perf" + str(maxmetagen) + ".dat"
                try:
                    line = open(path).readline()
                    values = line.split("\t")
                    values = [float(v) for v in values[:-1]] # -1 is for \n
                except Exception as e:
                    print(e)
                    break
                max_index = int(np.argmax(values)) + 1 # +1 because there is the mean meta-genotype as well
                print("selecting ",max_index)
        if condition == "cmaescheck":

            x = get_damagemaxperformance_from_file_cmaescheck(args.DEST + "/cmaescheck/exp" + replicate,
                                                              function_evaluations, slicing_factor=slicing_factor)
        else:

            filename = get_file_name_test(args.DEST, condition, replicate, test_type)
            # filename,n_pop,function_evaluations, slicingfactor, strip_weights=False,type="dimension"
            _w, x = get_damagemaxperformance_from_file_pop(filename, n_pop,
                                                           strip_weights,test_type)
            # assert len(x) == n_pop


            for d in range(num_damages):
                additional = funevs - len(x[max_index][d])
                if additional:
                    last_el = x[max_index][d][-1]
                    x[max_index][d] += [last_el for i in range(additional)]
                else:
                    x[max_index][d] = x[max_index][d][0:funevs]
                lines[i][d] = x[max_index][d][0:funevs]  # +1 because first is mean meta-genotype
    # all done, now aggregate and add to lines to plot
    m=np.mean(lines,axis=(0,1)) - MAXFIT
    mean_lines.append(m)
    means=np.mean(lines,axis=1)
    sd=np.std(means,axis=0) /np.sqrt(len(replicates)) # SD over replicates
    sd_lines1.append(m-sd)
    sd_lines2.append(m+sd)


def get_performances():



    for j, t in enumerate(test_types):
            mean_lines = []
            sd_lines1 = []
            sd_lines2 = []
            for cond in conditions:

                c, pop, step = cond
                print(c)
                if "meta" in c:
                    n_pop = 10
                    strip_weights=True
                else:
                    n_pop = 1
                    strip_weights=False
                get_damageperformances_pop(mean_lines,sd_lines1,sd_lines2, n_pop, c, t, replicates,strip_weights=strip_weights)

            colors = ["C5","C0", "C1", "C2", "C3", "C4"]  # colors for the lines
            # (numsides, style, angle)
            markers = ["*","o", "^", "s", "X", "D"]  # markers for the lines

            time=np.array(range(1,function_evaluations+1,1))
            createPlot(mean_lines, x_values=time,
                           save_filename=args.DEST+"/"+t+"test.pdf", legend_labels=labels,
                           colors=colors, markers=markers, xlabel="Function evaluations", ylabel="Best test-performance ($m$)",
                           xlim=[0, time[-1] + 5], xscale="linear", yscale="linear", ylim=None,
                           legendbox=None, annotations=[], xticks=[], yticks=[], task_markers=[], scatter=False,
                           legend_cols=1, legend_fontsize=26, legend_indexes=[], additional_lines=[], index_x=[],
                           xaxis_style="plain", y_err=[], force=True, fill_between=(sd_lines1, sd_lines2),legend_setting="empty")

            createPlot(mean_lines, x_values=time,
                           save_filename=args.DEST+"/"+t+"test_legend.pdf", legend_labels=labels,
                           colors=colors, markers=markers, xlabel="Function evaluations", ylabel="Best test-performance ($m$)",
                           xlim=[0, time[-1] + 5], xscale="linear", yscale="linear", ylim=None,
                           legendbox=None, annotations=[], xticks=[], yticks=[], task_markers=[], scatter=False,
                           legend_cols=1, legend_fontsize=26, legend_indexes=[], additional_lines=[], index_x=[],
                           xaxis_style="plain", y_err=[], force=True, fill_between=(sd_lines1, sd_lines2),legend_setting="separate")



if __name__ == "__main__":
    get_performances()