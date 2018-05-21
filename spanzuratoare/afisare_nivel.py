#metoda ce primeste un parametru si afiseaza nivelul corespunzator parametrului primit
def afisare_nivel(nivel):
    f=open("nivele/" + str(nivel) + '.txt', 'r') #afiseaza textul corespunzator nivelului cerut
    print(f.read())
    f.close()

