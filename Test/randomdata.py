import random

# 文件路径
file_path = 'data.txt'

# 读取文件内容，并去除每行末尾的换行符
with open(file_path, 'r', encoding='utf-8') as file:
    lines = [line.rstrip('\n') for line in file]

# 打乱行的顺序
random.shuffle(lines)

# 将打乱顺序后的行写回到文件
with open(file_path, 'w', encoding='utf-8') as file:
    file.write('\n'.join(lines) + '\n')  # 使用'\n'.join()来合并行，并在最后添加一个换行符