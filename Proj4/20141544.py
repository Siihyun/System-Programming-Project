import requests
from bs4 import BeautifulSoup


visited = []
count = 1
site= "http://cspro.sogang.ac.kr/~gr120170213/"

def get(url):
    global count
    global site
    global visited
    if url in visited: #check the site already has been visited
        return
    else:
        try:
            req = requests.get(url)
            if req.ok == False: # if the connection is failed
                return
            visited += [url] # add site to list
            c = req.content 
            BS = BeautifulSoup(c,"html.parser")
            fp = open("Output_"+"%04d" %count +".txt", "wt") # file open
            fp.writelines(BS.get_text())
            fp.close()
            count = count + 1

            for i in BS.find_all('a'):
                string = i.get('href') # get tags

                if( string != "" and '?' not in string and '#' not in string): 
                    if( string.find(site) == -1): # if url was not visited
                        string = site + string 

                    get(string) #visit the page recursively
        except:
            pass
first_site = site + "index.html"
get(first_site)
fp2 = open("URL.txt", "w")
for i in range(0, len(visited)-1):
    fp2.writelines(visited[i])
    fp2.write("\n")
length = len(visited)
fp2.writelines(visited[length-1])
fp2.close()

