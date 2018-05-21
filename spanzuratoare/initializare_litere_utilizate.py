def initializare_litere_utilizate(cuvant_de_ghicit = [] , litere_utilizate = []):
    litere_utilizate.clear()
    litere_utilizate.append(cuvant_de_ghicit[0])
    if cuvant_de_ghicit[0] != cuvant_de_ghicit[len(cuvant_de_ghicit) - 1]:
        litere_utilizate.append(cuvant_de_ghicit[len(cuvant_de_ghicit) - 1])
