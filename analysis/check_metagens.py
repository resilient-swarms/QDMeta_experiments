from process_archive import *
import glob
import re

def check_metagens_forfuneval(folder,n_pop,fun_evals):
    path = folder + "/epochs_log.txt"
    metagens=0
    current_funeval=0
    if "epochrl" in folder:
        period=n_pop+1
    else:
        period=n_pop
    try:
        lines = read_spacedelimited(path)
        for n, line in enumerate(lines):

            if n % period == period - 1:
                new_funeval = int(line[1])
                if new_funeval <= current_funeval: # run was restarted
                    continue
                current_funeval = new_funeval
                metagens+=1 # otherwise count the metageneration
                if current_funeval>=fun_evals:
                    break # if you reach maximal fun_evals then finish
    except Exception as e:
        print(e)
        return
    if current_funeval < fun_evals:
        file=open("warnings.txt","a")
        file.write("WARNING: "+folder+" "+str(current_funeval)+ "<" + str(fun_evals) + " just taking the last metageneration\n")
    step = 20
    metagens=step * int(round(metagens / step))
    print(metagens)
    return metagens
def get_maxmetagen_in_folder(folder):

    list_of_files = glob.glob(folder+"/recovered_perf*.dat")
    max=0
    for file in list_of_files:
        num = file.replace(folder+"/recovered_perf","")
        num = int(num.replace(".dat",""))
        max = num if num > max else max  # set max = 0 before for-loop
    return max

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Process destination folder.')
    parser.add_argument('-d', dest='DEST', type=str)
    parser.add_argument('-e', dest='funevals', type=int)
    args = parser.parse_args()
    sys.stdout.write("%s\n"%(check_metagens_forfuneval(folder=args.DEST, n_pop=10, fun_evals=args.funevals),))