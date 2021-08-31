




import argparse
from check_metagens import *
import numpy as np

from filenames import *
from plots import *

parser = argparse.ArgumentParser(description='Process destination folder.')
parser.add_argument('-d', dest='DEST', type=str)

args = parser.parse_args()



COMPARE_META=False

# if COMPARE_META:
#     conditions = ["damage_meta_nonlinfm_added0genes_b1p1m2","damage_meta_nonlinfm_added0genes_b1p1",
#                   "damage_meta_selectionfm_added0genes_b1p1m4","damage_meta_selectionfm_added0genes_b1p1",
#                   "damage_meta_linfm_added0genes_mutationrl_b1p1m8", "damage_meta_linfm_added0genes_b1p1",
#                ]  # file system label for bd
#     labels = ["Meta NonLinear (Optimised)","Meta NonLinear",
#               "Meta Selection (Optimised)", "Meta Selection",
#                 "Meta Linear (Optimised)","Meta Linear"]  # labels for the legend
# else:
#     conditions = ["damage_meta_nonlinfm_added0genes_b1p1m2","damage_meta_nonlinfm_added0genes_b1p1","random_nonlinfm_","control2D_pos","control2D_pol", "control4D_jpa", "control6D_as"
#                ]  # file system label for bd
#     labels = ["Meta NonLinear (Optimised)","Meta","Random feature-map","Position", "Polar", "JointPairAngle",
#                      "AngleSum"]  # labels for the legend

if args.DEST.endswith("RHEX"):
    replicates = ["2", "3", "4", "5"]
    labels = ["Damage Feature-sets",
              #"Damage Feature-sets (no epoch control)",
              "Damage Trajectory",
              #"Damage Trajectory (no epoch control)",
              "Obstacle Feature-sets",
              #"Obstacle Feature-sets (no epoch control)",
              "Obstacle Trajectory",
             # "Obstacle Trajectory (no epoch control)"
              "AURORA",
             "CVT-ME"
            ]
    percentage_factor="10"
    conditions = [("damage_meta_featuresets_epochrl_b2p" + percentage_factor, 10,20),
                  #("damage_meta_featuresets_b1p" + percentage_factor, 10, 20),
                  ("damage_meta_epochrl_b2p" + percentage_factor, 10,20),
                  #("damage_meta_b1p" + percentage_factor, 10, 20),
                  ("envir_meta_featuresets_epochrl_b2p" + percentage_factor, 10,20),
                  #("envir_meta_featuresets_b1p" + percentage_factor, 10, 20),
                  ("envir_meta_epochrl_b2p" + percentage_factor, 10,20),
                ('AURORA_MLP',None,None),
                ("cvt50D_tra", None, None)
    ]
    N_g = 24
    N_BD = 50
    centroid_filename=os.environ["HOME"]+"/RHex_experiments/meta-cmaes/centroids/centroids_10_50.dat"
    genomultiplier=0.025 # sampled genotype
    #("envir_meta_b1p" + percentage_factor, 10,20)]

    #bd_shapes = [(4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4), (4096, 4)]  # shape of the characterisation

elif args.DEST.endswith("RASTRI"):
        replicates = [str(i+1) for i in range(20)]
        labels = ["QD-Meta Dimensionality Reduction",
                  "QD-Meta Translation",
                 "AURORA",
                  "CVT-ME"]
        conditions = [("dimension_meta_epochrl_b2p1", 10, 200),
                   ("trans_meta_epochrl_b2p1", 10, 200),
                   ("AURORA_MLP", None, None),
                   ("cvt_gen", None, None)]
        #bd_shapes = [(10000, 2), (10000, 2), (10000, 2),
        #             (10000,20)]  # shape of the characterisation
        N_g = 20
        N_BD = 20
        centroid_filename = os.environ["HOME"] + "/RHex_experiments/meta-cmaes/centroids/centroids_10000_20.dat"
        centroid_dimreduced_filename = os.environ["HOME"] + "/RHex_experiments/meta-cmaes/centroids/centroids_10000_18.dat"
        genomultiplier = 1.0  # evofloat genotype

else:
    raise Exception("specify a destination directory with RHEX or RASTRI")


def get_performance_metrics(filename):
    fit = []
    with open(filename, 'r') as f:
        for line in f:
            if line:  # avoid blank lines
                stripped = (line.strip()).split(" ")
                fit.append(float(stripped[0]))
    return fit

def get_performance_metrics_all(filename):
    fit = []
    genotype = []
    behaviour = []
    with open(filename, 'r') as f:
        for line in f:
            if line:  # avoid blank lines
                stripped = (line.strip()).split(" ")
                genotype.append([float(s) for s in stripped[0:N_g]])
                behaviour.append([float(s) for s in stripped[N_g:N_g + N_BD]])
                fit.append(float(stripped[N_g + N_BD]))
    return genotype, behaviour, fit



def get_performances_single(gens_list,behavs_list,fits_list,condition, replicates,test_type):
    gens=[]
    behavs=[]
    fitness=[]
    for replicate in replicates:
        filename = get_file_name_test(args.DEST, condition, replicate,test_type)
        print("will get max index from "+str(filename))
        if "centroid" in filename:
            gen, behav, fit = get_performance_metrics_all(filename)
            gens.append(gen)
            behavs.append(behav)
        else:
            fit = get_performance_metrics(filename)
        #assert len(x) == n_pop

        fitness.append(fit)

    if "centroid" in filename:
        gens_list.append(gens)
        behavs_list.append(behavs)
    fits_list.append(fitness)




def get_adaptperformance_from_file(filename,n_pop,only_mu=True):
    behaviour = []   # CMA-ES population mean at 0'th index
    genotype = []
    fitness = []

    p = 0
    weights = []
    features = []
    temp_features=[]
    temp_w = []
    temp_genotype = []
    temp_behaviour = []
    temp_fitness = []
    with open(filename, 'r') as f:
        for line in f:
            if line:  # avoid blank lines
                stripped = (line.strip())
                if len(features) == 0:
                    if stripped == "END FEATURES":
                        continue
                    elif stripped == "END FEATURES POPULATION MEAN":
                        features = temp_features
                    else:
                        temp_features.append(np.array(stripped.split(),dtype=float))
                elif len(weights) == 0:
                    if stripped == "END WEIGHTS":
                        continue
                    elif stripped == "END WEIGHTS POPULATION MEAN":
                        weights.append(temp_w)
                        temp_w = []
                    else:
                        temp_w.append(np.array(stripped.split(),dtype=float))
                elif len(weights) < n_pop + 1:  # collecting weights
                    if stripped == "END WEIGHTS":
                        weights.append(temp_w)
                        temp_w = []
                    else:
                        temp_w.append(np.array(stripped.split(),dtype=float))
                else:  # collecting performance data
                        if stripped == "END TEST META-INDIVIDUAL":
                            genotype.append(temp_genotype)
                            behaviour.append(temp_behaviour)
                            fitness.append(temp_fitness)
                            temp_fitness = []
                            temp_genotype = []
                            temp_behaviour = []


                        elif stripped == "END STATS POPULATION MEAN":
                            if only_mu: break
                            continue
                        else:
                            values = stripped.split()
                            # temp_bin.append(float(values[0])* 100.0/MAXBINS) # percentage)
                            # temp_pwd.append(float(values[1]))
                            if "centroid" in filename:
                                temp_genotype.append([float(g) for g in values[0:N_g]])
                                temp_behaviour.append([float(b) for b in values[N_g:N_g + N_BD]])
                                temp_fitness.append(float(values[N_g + N_BD]))

                            else:
                                temp_fitness.append(float(values[0]))

    return features, weights, genotype, behaviour, fitness

def get_adaptperformance_from_file_control(filename):
    fitness = []
    with open(filename, 'r') as f:
        for line in f:
            if line:  # avoid blank lines
                stripped = (line.strip())
                values = stripped.split()
                # bins.append(float(values[0])* 100.0/MAXBINS) # percentage)
                # pwd.append(float(values[1]))
                fitness.append(float(values[0]))
    return fitness

def get_adaptperformance_and_behaviour_from_file_control(filename):
    fitness = []
    genotype =[]
    behaviour=[]
    with open(filename, 'r') as f:
        for line in f:
            if line:  # avoid blank lines
                stripped = (line.strip())
                values = stripped.split()
                genotype.append([float(g) for g in values[0:N_g]])
                behaviour.append([float(b) for b in values[N_g:N_g+N_BD]])
                fitness.append(float(values[N_g+N_BD]))
    return genotype,behaviour,fitness

def get_weight_hist(n_pop, condition, test_type, replicates):
    #if type!="test": assert(selection_criterion is None)
    frequency_dict={"Position": 0,"Polar":0,"JointPairAngle":0,"AngleSum":0}
    def feature2group(feature):
        if feature < 2:
            return "Position", 1/2
        elif feature < 4:
            return "Polar" , 1/2
        elif feature < 8:
            return "JointPairAngle", 1/4
        elif feature < 14:
            return "AngleSum", 1/6
        else:
            raise Exception("feature index > 14, ", feature)
    max_features=[]

    for replicate in replicates:
            f = get_file_name_test(args.DEST, condition, replicate,test_type)
            print("will get max index from "+str(f))
            w, _c, _pwd, _f = get_adaptperformance_from_file(f, n_pop)
            w=w[0]
            # see l. 255-294 in fitbottom for index order (pos, pol, jpa, as)
            #pos
            D=4
            for i in range(D):
                feature=np.argmax(w[i])
                max_features.append(feature)
                max_group, increment = feature2group(feature)

                frequency_dict[max_group] += increment
    #         temp_w=w[0][0:2]
    #         temp_w=np.append(temp_w,w[1][0:2])
    #         temp_w=np.append(temp_w,w[2][0:2])
    #         temp_w = np.append(temp_w, w[3][0:2])
    #         joined_w["pos"] = np.append(joined_w["pos"], temp_w)
    #         PLT.hist(list(temp_w), bins=5, density=True)
    #         PLT.savefig(args.DEST + "/histPOS"+condition+str(replicate)+".pdf")
    #         PLT.close()
    #         #pol
    #         temp_w=w[0][2:4]
    #         temp_w=np.append(temp_w,w[1][2:4])
    #         temp_w=np.append(temp_w,w[2][2:4])
    #         temp_w = np.append(temp_w, w[3][2:4])
    #         joined_w["pol"] = np.append(joined_w["pol"], temp_w)
    #         PLT.hist(list(temp_w), bins=5, density=True)
    #         PLT.savefig(args.DEST + "/histPOL"+condition+str(replicate)+".pdf")
    #         PLT.close()
    #         #jpa
    #         temp_w=w[0][4:8]
    #         temp_w=np.append(temp_w,w[1][4:8])
    #         temp_w=np.append(temp_w,w[2][4:8])
    #         temp_w = np.append(temp_w, w[3][4:8])
    #         joined_w["jpa"] = np.append(joined_w["jpa"], temp_w)
    #         PLT.hist(list(temp_w), bins=5, density=True)
    #         PLT.savefig(args.DEST + "/histJPA"+condition+str(replicate)+".pdf")
    #         PLT.close()
    #         #as
    #         temp_w = w[0][8:14]
    #         temp_w = np.append(temp_w, w[1][8:14])
    #         temp_w = np.append(temp_w, w[2][8:14])
    #         temp_w = np.append(temp_w, w[3][8:14])
    #         joined_w["as"] = np.append(joined_w["as"], temp_w)
    #         PLT.hist(list(temp_w), bins=5, density=True)
    #         PLT.savefig(args.DEST + "/histAS" + condition + str(replicate) + ".pdf")
    #         PLT.close()
    #
    # weight_sums=[]
    # for D,W in joined_w.items():
    #     print(D+str(np.sum(W)))
    #     PLT.hist(list(W), bins=5, density=True)
    #     PLT.savefig(args.DEST + "/histJOINED"+D+condition+".pdf")
    #     PLT.close()
    print(max_features)
    PLT.hist(max_features)
    PLT.savefig("histogram_features.pdf")
    PLT.close()
    print(np.array(list(frequency_dict.values()))/sum(frequency_dict.values()))
    PLT.bar(list(frequency_dict.keys()),np.array(list(frequency_dict.values()))/sum(frequency_dict.values()))
    PLT.savefig("histogram_group.pdf")
    PLT.close()
    #     PLT.savefig(args.DEST + "/histJOINED"+D+condition+".pdf")
    #     PLT.close()
def get_featuremaps():
    n_input=48
    n_output=4
    get_featuremap_vis(n_input,n_output,replicates=list(range(1,21)),elbow_test=False)
def get_featuremap_vis(n_input, n_output, replicates,elbow_test=False):
    full_featurelist = []
    learner_indexes=[[] for c in conditions]
    start_index=0
    for c, cond in enumerate(conditions):
        bd, npop, step = cond
        for replicate in replicates:
                f = get_file_name_test(args.DEST, bd, replicate,test_type="damage") #note that test type does not matter here
                features, _w, _c, _pwd, _f = get_adaptperformance_from_file(f, npop)
                full_featurelist+=list(features)
                next_index=len(full_featurelist)
                learner_indexes[c].append(range(start_index, next_index))
                start_index=next_index
    from sklearn.cluster import KMeans
    n_clust=8
    inputfeaturelist=[feature[0:n_input] for feature in full_featurelist]
    outputfeaturelist = [feature[n_input:n_input+n_output] for feature in full_featurelist]

    if elbow_test:
        input_losses=[]
        output_losses=[]
        K=range(1,26)
        for i in K:
            print(i)
            input_kmeans = KMeans(n_clusters=i, random_state=0,verbose=0).fit(inputfeaturelist)
            input_losses.append(input_kmeans.inertia_)
            output_kmeans = KMeans(n_clusters=i, random_state=0,verbose=0).fit(outputfeaturelist)
            output_losses.append(output_kmeans.inertia_)
        PLT.plot(K, input_losses, 'bx-')
        PLT.xlabel('Number of clusters')
        PLT.ylabel('Sum squared error')
        PLT.savefig("clusters_input.pdf")
        PLT.close()
        PLT.plot(K, output_losses, 'bx-')
        PLT.xlabel('Number of clusters')
        PLT.ylabel('Sum squared error')
        PLT.savefig("clusters_output.pdf")
        return
    else:
        input_kmeans = KMeans(n_clusters=n_clust, random_state=0,verbose=0).fit(inputfeaturelist)
        output_kmeans = KMeans(n_clusters=n_clust, random_state=0,verbose=0).fit(outputfeaturelist)
    


    def binning(features,dx=0.10):
        for i in range(len(features)):
            features[i] = int(features[i] / dx)
        return tuple(features)
    n_plots=(len(conditions)+1)
    figx = 10*n_plots
    figy = 10
    fig = PLT.figure(figsize=(figx, figy))
    counter=1
    for c,condition in enumerate(conditions):
        ax = fig.add_subplot(1,n_plots,counter)
        counter+=1
        condition, npop, step = condition
        inputfreqfile = open(args.DEST + "/inputfreq_" + condition + ".dat","w")
        outputfreqfile=open(args.DEST+"/outputfreq_"+condition+".dat","w")
        combinedfreqfile = open(args.DEST + "/combinedfreq_" + condition + ".dat","w")


        inputcounts=[[] for l in input_kmeans.cluster_centers_]
        for label, _center in enumerate(input_kmeans.cluster_centers_):

            for rep_indexes in learner_indexes[c]:
                inputcount = 0.0
                for i in rep_indexes:
                    if input_kmeans.labels_[i] == label:
                        inputcount+=1

                inputfreqfile.write(str(label) + " " + str(inputcount)+ "\n")
                inputcounts[label].append(inputcount)
        outputcounts=[[] for l in input_kmeans.cluster_centers_]
        for label,_center in enumerate(output_kmeans.cluster_centers_):

            for rep_indexes in learner_indexes[c]:
                outputcount = 0.0
                for i in rep_indexes:
                    if output_kmeans.labels_[i] == label:
                        outputcount+=1

                outputfreqfile.write(str(label) + " " + str(outputcount) + "\n")
                outputcounts[label].append(outputcount)
        combinedcounts=[[[] for l in output_kmeans.cluster_centers_] for k in input_kmeans.cluster_centers_]
        for label_i,_center_i in enumerate(input_kmeans.cluster_centers_):
            for label_o, _center_j in enumerate(output_kmeans.cluster_centers_):
                for rep_indexes in learner_indexes[c]:
                    combinedcount = 0.0
                    for i in rep_indexes:
                        if input_kmeans.labels_[i] == label_i and output_kmeans.labels_[i] == label_o:
                            combinedcount+=1

                    combinedfreqfile.write("("+str(label_i) + "," + str(label_o) +")"+ " " + str(combinedcount)+ "\n")
                    combinedcounts[label_i][label_o].append(combinedcount)


        #im = ax.imshow(np.mean(np.array(combinedcounts),axis=2))
        pcm = ax.pcolormesh(np.mean(np.array(combinedcounts),axis=2), cmap='viridis',vmin=0,vmax=300)  # viridis is friendly colorblind friendly

        # Loop over data dimensions and create text annotations.
        for i in range(n_clust):
            for j in range(n_clust):
                mean = np.mean(combinedcounts[i][j])
                sd   = np.std(combinedcounts[i][j])
                text = ax.text(j+0.5,i+0.5, r"$%.1f \pm %.1f$"%(mean,sd),
                               ha="center", va="center", color="w")
        # We want to show all ticks...
        ax.set_xticks(np.arange(n_clust))
        ax.set_yticks(np.arange(n_clust))
        # ... and label them with the respective list entries
        ax.set_xticklabels(["Input cluster "+str(i) for i in range(1,n_clust+1)],fontsize=10)
        ax.set_yticklabels(["Output cluster "+str(i) for i in range(1,n_clust+1)],fontsize=10)
        PLT.setp(ax.get_xticklabels(), rotation=+30, ha="right",
                     rotation_mode="anchor")
    ax = fig.add_subplot(1, n_plots, n_plots)
    fig.colorbar(pcm)
    ax.remove() # remove empty plot
    PLT.tight_layout()
    PLT.savefig(args.DEST + "/feature_heatmap.pdf")
    PLT.close()

def get_weight_hist_expected():

    temp_w=np.random.random((100000,15))
    for i in range(len(temp_w)):
        temp_w[i]/=sum(temp_w[i])
    PLT.hist(list(temp_w.flatten()), bins=20)
    PLT.savefig(args.DEST + "/Results/random.pdf")
    PLT.close()
    # PLT.hist(w[max_index][6:12], bins=range(0, 0.40, 0.05))
    # PLT.hist(w[max_index][], bins=range(0, 0.40, 0.05))



def get_performances_pop(gens_list,behavs_list,fitness_list,n_pop, condition, replicates, test_type, only_mu=False):


    #if type!="test": assert(selection_criterion is None)
    genotype=[]
    behav=[]
    fitness=[]
    for replicate in replicates:
        folder = get_output_folder_test(args.DEST, condition, replicate)
        print("will get max index from "+str(folder))
        if args.DEST.endswith("RASTRI"):
            # stopped exactly at 100 mill --> get last
            maxmetagen = get_maxmetagen_in_folder(folder)
        else:
            maxmetagen = check_metagens_forfuneval(folder,
                                                   n_pop=n_pop, fun_evals=12 * 10 ** 6)
        path = folder + "/recovered_perf" + str(maxmetagen) + ".dat"
        try:
            line = open(path).readline()
            values = line.split("\t")
            values = [float(v) for v in values[:-1]]  # -1 is for \n
        except Exception as e:
            print(e)
            break
        max_index = int(np.argmax(values)) + 1  # +1 because there is the mean meta-genotype as well
        print("selecting ", max_index)
        f = get_file_name_test(args.DEST, condition, replicate, test_type)
        _f, _w, g, b, fit = get_adaptperformance_from_file(f, n_pop,only_mu)
        fitness.append(fit[max_index])
        genotype.append(g[max_index])
        behav.append(b[max_index])


    gens_list.append(genotype)
    behavs_list.append(behav)
    fitness_list.append(fitness)


def get_centroids():
    centroids=[]
    with open(centroid_filename, 'r') as f:
        for line in f:
            values = line.split(" ")
            values = [float(v) for v in values[:-1]]  # -1 is for \n
            centroids.append(np.array(values))
    return centroids

def get_archive(genotypes, behaviours,fitness,centroids):
    archive={}
    for i, behaviour in enumerate(behaviours):
        # find closest matching centroid
        mindistc = -1
        mindist = float("inf")
        for c , centroid in enumerate(centroids):
            dist = np.sum(np.square(behaviour - centroid))
            if dist < mindist:
                mindistc=c
                mindist=dist
        # increase frequency of this centroid and if it is the best add the solution to the archive
        current_data = archive.get(mindistc,None)
        if current_data is None:
            archive[mindistc] = (1,genotypes[i],behaviour,fitness[i])
        else:
            freq, best_geno, best_behav, best_fit = archive[mindistc]
            freq+=1
            if fitness[i] > best_fit:
                archive[mindistc] = (freq,genotypes[i],behaviour,fitness[i])
            else:
                archive[mindistc] = (freq,best_geno,best_behav,best_fit)
    return archive
def get_performances(test_type):
    def flatten(t):
        return [item for sublist in t for item in sublist]
    n_pop=10
    genotype_list=[]
    behav_list=[]
    fitness_list=[]
    centroids = get_centroids()

    for con in conditions:
        con,_,_ = con
        if "meta" in con:
            get_performances_pop(genotype_list, behav_list,fitness_list, n_pop, con, replicates, test_type, only_mu=False)
        else:
            # mins,means,maxs, condition,test_type,replicates, type
            get_performances_single(genotype_list, behav_list,fitness_list, con, replicates, test_type)

        archive = get_archive(flatten(genotype_list[-1]), flatten(behav_list[-1]), flatten(fitness_list[-1]), centroids)
        centroid_stats = open("centroid_stats_"+con+".txt", "w")
        centroid_stats.write("coverage %d \n"%(len(archive)))
        centroid_stats.write("centroid\tfreq\tgeno\tbehaviour\n")
        for c, val in archive.items():
            freq, best_geno, best_behav, best_fit = val
            centroid_stats.write("%d | %d "%(c,freq))
            centroid_stats.write("|")
            for g in best_geno:
                centroid_stats.write("%.3f "%(g*genomultiplier))
            centroid_stats.write("|")
            for b in best_behav:
                centroid_stats.write("%.3f " % (b))
            centroid_stats.write("|")
            centroid_stats.write("%.3f " % (best_fit))
            centroid_stats.write("\n")
    # offsets=20
    # legs=8
    # total=offsets*legs
    # leg=0
    # for index in range(0,total,offsets):
    #     leg+=1
    #     y=[]
    #     bottom=[]
    #     top=[]
    #     for c, cond in enumerate(conditions):
    #         data=np.array([covs_list[c][r][index:index+offsets] for r,run in enumerate(replicates)])
    #         m = np.mean(data,axis=0)
    #         s = np.std(data,axis=0)
    #         y.append(m)
    #         bottom.append(m - s)
    #         top.append(m + s)
    #     createPlot(y,x,colors=["C"+str(i) for i in range(len(conditions))],markers=markers,xlabel="Offset (degrees)",ylabel="Targets reached (%)",
    #                ylim=[0,100],save_filename="testperformance_leg"+str(leg)+"_"+test_type+"_dynamic.pdf",legend_labels=labels,
    #                xlim=[-200,200],xticks=[-180,-90,0,90,180],force=True,fill_between=[bottom,top],
    #                legend_setting="default")
    # with open("test_performances_coverage.txt", "w") as f:
    #     make_table(f, covs_list,
    #                rowlabels=labels,
    #                columnlabels=[],
    #                conditionalcolumnlabels=[("Mean", "float2")],
    #                transpose=True)
    # with open("test_performances_pwds.txt", "w") as f:
    #     make_table(f, pwds_list,
    #                rowlabels=labels,
    #                columnlabels=[],
    #                conditionalcolumnlabels=[("Mean", "float2")],
    #                transpose=True)
    print("damage")
    meanfitness_list=[[np.mean(replicate) for replicate in condition] for condition in fitness_list]
    print(meanfitness_list)
    make_significance_table(meanfitness_list,0)   # damage -> comp to damage

    # print(type)
    # print()
    # print("envir")
    # make_significance_table(means[1],0)  # envir --> comp to envir
    # make_significance_table(means[4],1)   # envir --> comp to envir

def make_significance_table(means,comp_index):
        tablefile=open("signif_table.txt","w")
        simpletablefile = open("table.txt", "w")
        for j, condition in enumerate(labels):
            tablefile.write(condition + " & ")
        tablefile.write(" \n ")
        for j, condition in enumerate(labels):
            if comp_index == j:
                tablefile.write(r"$ %.3f \pm %.2f $ &"%(np.mean(means[j]),np.std(means[j])))
                simpletablefile.write(r"$ %.3f \pm %.2f $ &" % (np.mean(means[j]), np.std(means[j])))
                continue
            x=np.array(means[comp_index]).flatten()
            y=np.array(means[j]).flatten()

            U, p = ranksums(x, y)
            p_value = "$p=%.3f $" % (p) if p > 0.001 else r"p<0.001"
            delta,label = cliffs_delta(x,y)
            delta_value = r"$\mathbf{%.2f}$"%(delta) if label == "large" else r"$%.2f$"%(delta)
            tablefile.write(r"$ %.3f \pm %.2f $ &"%(np.mean(means[j]),np.std(means[j])))
            simpletablefile.write(r"$ %.3f \pm %.2f $ &" % (np.mean(means[j]), np.std(means[j])))
            tablefile.write(p_value + ", " + delta_value + " &")
            print( labels[comp_index] + "vs" + condition)
            print(p_value,delta_value)

def cliffs_delta(x,y):
    """
    meaning: proportion x>y minus proportion y>x
    |d|<0.147 "negligible", |d|<0.33 "small", |d|<0.474 "medium", otherwise "large"
    here calculate based on relation with the rank-sum test
    :param U: the result of the Wilcoxon rank-test/Mann-Withney U-test
    :return:
    """
    m=len(x)
    n=len(y)
    print("starting with lengths %d %d " % (m, n))
    # delta =  2.0*U/float(m*n) - 1.0
    if len(x) > 2000 or len(y)>2000:   # avoid memory issues
        print("digitising samples")
        xspace=np.linspace(x.min(),x.max(),500)
        yspace = np.linspace(x.min(), x.max(), 500)
        freq_x=np.histogram(x, bins=xspace)[0]
        freq_y=np.histogram(y, bins=yspace)[0]

        count=0
        for i in range(len(freq_x)):
            for j in range(len(freq_y)):
                num_combos=freq_x[i]*freq_y[j]
                xx=xspace[i]
                yy=yspace[j]
                if xx > yy:
                    count+=num_combos
                else:
                    count-=num_combos
        count/=float(m*n)
    else:
        z=np.array([np.array(xx) - np.array(yy) for xx in x for yy in y]) # consider all pairs of data
        count=float(sum(z>0) - sum(z<0))/float(m*n)
    # assert count==delta, "delta:%.3f  count:%.3f"%(delta,count)
    label = None
    magn=abs(count)
    if magn < 0.11:
        label="negligible"
    elif magn < 0.28:
        label="small"
    elif magn < 0.43:
        label="medium"
    else:
        label="large"
    return count, label
if __name__ == "__main__":
    #get_featuremaps()
    #get_performances("test_envir_damage")
    get_performances("train_envir_centroid_damage")
    #get_performances("train_damage_centroid_damage")
    #get_performances("envir")

    #get_weight_hist(5, "damage_meta_linfm_added0genes_b1p1", "damage", replicates)
    # #get_weight_hist(5, "damage_meta", "damage", replicates)
    # get_weight_hist_expected()
    #get_adaptperformance_from_file("/home/david/Docker/singularity/planar_metacmaes/singularity/Results/control2D_pol/exp5/test_damage_performance", n_pop)

    #get_performances()

