groupno = 24
files = []
for i in [1, 2, 4, 7, 9]:
    files.append("Q{}/Assgn1_{}_{}.sh".format(i, i, groupno))
# files = ["Q7/Assgn1_7_24.sh"]
wc = 0
for file in files:
    with open(file) as f:
        content = f.read().replace('=', ' = ').replace('|', ' | ').replace(';', ' ; ').replace(',', ' , ').replace('>', ' > ').replace('<', ' < ')
        wc += len(content.split())
        print(file, len(content.split()))
print("Total: ", wc)