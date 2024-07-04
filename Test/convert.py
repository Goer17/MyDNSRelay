import csv

# 输入CSV文件的路径
input_csv_path = 'top.chinaz.com.ip.csv'
# 输出TXT文件的路径
output_txt_path = 'data.txt'

# 打开CSV文件并读取数据
with open(input_csv_path, mode='r', newline='', encoding='utf-8') as csv_file:
    csv_reader = csv.reader(csv_file)
    # 读取CSV文件中的每一行
    with open(output_txt_path, mode='w', encoding='utf-8') as txt_file:
        for row in csv_reader:
            # 只取第一列数据
            first_column = row[0]
            # 写入TXT文件
            txt_file.write(first_column + '\n')