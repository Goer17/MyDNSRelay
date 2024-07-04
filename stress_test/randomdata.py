import random

file_path = 'data.txt'

with open(file_path, 'r', encoding='utf-8') as file:
    lines = [line.rstrip('\n') for line in file]

random.shuffle(lines)

with open(file_path, 'w', encoding='utf-8') as file:
    file.write('\n'.join(lines) + '\n') 
