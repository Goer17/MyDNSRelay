def copy_lines(input_file, output_file):
    with open(input_file, 'r', encoding='utf-8') as infile:
        with open(output_file, 'w', encoding='utf-8') as outfile:
            lines = infile.readlines()
            for line in lines[:300]:  # Read the first 100 lines
                line = line.strip()  # Strip newline characters and spaces
                for _ in range(10):  # Copy each line ten times
                    outfile.write(line + '\n')

    print(f"The first 300 lines have been copied and saved in {output_file}.")

# Example usage
if __name__ == "__main__":
    input_file = 'unique.txt'  # Replace with your input file name
    output_file = 'domain.txt'  # Replace with your output file name
    copy_lines(input_file, output_file)
