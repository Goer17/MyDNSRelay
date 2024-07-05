import subprocess

def check_timeout(domain):
    try:
        result = subprocess.run(['nslookup', domain], capture_output=True, timeout=5, text=True)
        if "timed out" in result.stdout:
            return False
        else:
            return True
    except subprocess.TimeoutExpired:
        return False
    except Exception as e:
        print(f"Error checking {domain}: {e}")
        return False

def main(input_file, output_file):
    domains = []
    with open(input_file, 'r') as f:
        domains = f.read().splitlines()

    non_timeout_domains = []
    for domain in domains:
        if len(non_timeout_domains) >= 300:
            break
        if check_timeout(domain):
            non_timeout_domains.append(domain)

    with open(output_file, 'w') as f:
        for domain in non_timeout_domains:
            f.write(domain + '\n')

if __name__ == "__main__":
    input_file = 'unique.txt'
    output_file = 'domain.txt'
    main(input_file, output_file)
