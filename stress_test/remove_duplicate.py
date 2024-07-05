def remove_duplicates(input_file, output_file):
    seen_lines = set()

    with open(input_file, 'r', encoding='utf-8') as infile:
        with open(output_file, 'w', encoding='utf-8') as outfile:
            for line in infile:
                line = line.strip()  # Strip newline characters and spaces
                if line not in seen_lines:
                    seen_lines.add(line)
                    outfile.write(line + '\n')

    print(f"Duplicates have been removed from {input_file}. Results saved in {output_file}.")

if __name__ == "__main__":
    input_file = 'data.txt'  # Replace with your input file name
    output_file = 'unique.txt'  # Replace with your output file name
    remove_duplicates(input_file, output_file)
