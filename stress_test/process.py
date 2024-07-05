import random

def expand_and_shuffle(input_file, output_file):
    domains = []
    with open(input_file, 'r') as f:
        domains = f.read().splitlines()

    expanded_domains = []
    for domain in domains:
        expanded_domains.extend([domain] * 10)

    random.shuffle(expanded_domains)

    with open(output_file, 'w') as f:
        for domain in expanded_domains:
            f.write(domain + '\n')

if __name__ == "__main__":
    input_file = 'domain.txt'
    output_file = 'domain.txt'
    expand_and_shuffle(input_file, output_file)
