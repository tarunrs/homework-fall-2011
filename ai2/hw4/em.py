import math
import sys

def populate_array(filename):
    with open(filename) as f:
        a = [float(l.strip()) for l in f.readlines()]
    return a

def calculate_mean(a):
    sum = 0.0
    for number in a:
        sum = sum + number
    return (sum/len(a))

def standard_deviation(a, mean):
    sum = 0
    num_elements = len(a)
    for number in a:
        sum = sum + pow((float(number) - mean), 2)
    return math.sqrt(sum / (num_elements - 1) )
    

def probability (x, mean, std_dev):
    expo = ((x - mean) ** 2) / (2 * (std_dev ** 2))
    coeff = 1.0 / (std_dev * (math.sqrt(2 * math.pi)))
    prob = coeff * math.exp(-expo)
    return prob 
            

def expectation(points, weight, means, sds):
    lst = []
    for i in range(len(weight)):
        prob = []
        for pt in points:
            p = probability(pt, means[i], sds[i])
            prob.append(p * weight[i])
        lst.append(prob)
    return lst
    
def normalize_probabilities(list_probs, num_gauss, num_elements):
    for i in range(num_elements):
        sum = 0.0
        for j in range(num_gauss):
            sum = sum + list_probs[j][i]
        for j in range(num_gauss):
            list_probs[j][i] = list_probs[j][i]/sum

def calculate_logs (list_probs, num_gauss, num_elements):
    total_sum = 0.0
    #print num_gauss, num_elements
    for i in range(num_elements):
        log_sum = 0.0
        for j in range(num_gauss):
            log_sum = log_sum + list_probs[j][i]
        log_sum = math.log(log_sum, 2)
        #print log_sum
        total_sum = total_sum + log_sum
    return total_sum

def update_means(x, list_probs, means, weights):
    for i in range(len(means)):
        sum = 0.0
        sum_prob = 0.0
        for j in  range(len(x)):
            sum = sum + (list_probs[i][j] * x[j])
            sum_prob = sum_prob + list_probs[i][j]
        means[i] = sum/sum_prob
        weights[i] = sum_prob

def update_std_dev(x, list_probs, std_devs, means):
    for i in range(len(means)):
        sum = 0.0
        sum_prob = 0.0
        for j in  range(len(x)):
            sum = sum + (list_probs[i][j] * ((x[j] - means[i] ) ** 2))
            sum_prob = sum_prob + list_probs[i][j]
        std_devs[i] = math.sqrt(sum/sum_prob)

def alpha_normalize(x, list_probs, num):
    for i in range(len(x)):
        sum = 0.0
        for n in range(num):
            sum = sum + list_probs[n][i]
        for n in range(num):
            list_probs[n][i] = list_probs[n][i] / sum
        
def update_weights(wts):
    for i in range(len(wts)):
        wts[i] = wts[i]/ 1000.0

def print_usage():
    print """USAGE:
        python em.py <infile>
        Example: python em.py fileA"""

if len(sys.argv) == 2:
    input_filename = sys.argv[1]
    output_filename = "out" + input_filename
else:
    
    print_usage()
    quit()

start_gauss_num = 1
end_gauss_num = 10
iterations = 1000
x = populate_array(input_filename)
mean =  sum(x)/len(x)
std_dev = standard_deviation(x, mean)
f = open(output_filename,"w")
output_logs = []
for num_gauss in range(start_gauss_num, end_gauss_num):
    wts = [ 1.0/num_gauss for i in range(num_gauss)]
    means = [ (mean + (i - num_gauss/2) * 10 )for i in range(num_gauss)]
    std_devs = [10 for i in range(num_gauss)]

    logs = []
    for i in range(iterations):
        probs = expectation(x, wts, means, std_devs)
        c_log = calculate_logs(probs, num_gauss, len(x))
        alpha_normalize(x, probs, num_gauss)
        normalize_probabilities(probs, num_gauss, len(x))
        update_means(x, probs, means, wts)
        update_std_dev(x, probs, std_devs, means)
        update_weights(wts)
        logs.append(c_log)
    output_logs.append(logs)
    print "Number of Gaussians : ", num_gauss
    print "Means : " , means
    print "Standard deviations : " , std_devs
    print "Weights : ", wts
    print "Log likelihood : ", logs[-1]
    print " -----------------------------------------------------------------------------"

for i in range(iterations):
    temp = ""
    for j in range(start_gauss_num, end_gauss_num):
        temp = temp + str (output_logs[j-start_gauss_num][i]) + " "
    temp = temp + "\n"    
    f.write(temp)
    
f.close()
        
