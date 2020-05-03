import tarfile

def main():
    tar = tarfile.open("animals.tar")
    total = 0
    for member in tar.getmembers():
        print(member.name + ": " + str(member.size))
        total += member.size

    print(total)

main()