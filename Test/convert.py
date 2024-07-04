import csv
input_csv_path = 'top.chinaz.com.ip.csv'

_txt_path = 'data.txt'
with open(input_csv_path, mode='r', newline='', encoding='utf-8') as csv_file:
    csv_reader = csv.reader(csv_file)
    with open(output_txt_path, mode='w', encoding='utf-8') as txt_file:
        for row in csv_reader:
            first_column = row[0]
            # 写入TXT文件
            txt_file.write(first_column + '\n')
