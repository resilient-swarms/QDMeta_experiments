
import numpy as np


def get_output_folder_test(folder,condition,replicate):


    return folder+"/"+condition+"/exp"+str(replicate)



def get_file_name_test(folder,condition,replicate,test_type):

    Outfolder=get_output_folder_test(folder,condition,replicate)
    return Outfolder+"/"+test_type+"_performance"


def get_file_name_train(folder, condition, test_type, replicate):
    Outfolder = get_output_folder_test(folder, condition, replicate)
    return Outfolder + "/" + test_type + "_trainperformance"

def get_file_name_metafitness(folder, condition, test_type, replicate,generation):
    Outfolder = get_output_folder_test(folder, condition, replicate)
    return Outfolder + "/recovered_perf"+str(generation)+".dat"

def get_archive_plus_dir(folder,condition,time,index=None):
    """

    """
    if "meta" in condition:
        archive_tag=str(time)+"_archive"+str(index)+".dat"
    else:
        archive_tag="archive_"+str(time)+".dat"
        #last_gen="1500"

    Outfolder=folder+"/"+condition+"/exp"
    return archive_tag ,Outfolder