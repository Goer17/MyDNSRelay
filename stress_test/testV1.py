import subprocess
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from tqdm import tqdm


# Function to perform the nslookup query
def query_domain(domain, ip_address):
    command = ['nslookup', domain, ip_address]
    try:
        result = subprocess.run(command, capture_output=True, text=True, check=True, timeout=5)
        return f"{domain}: {result.stdout}"
    except subprocess.CalledProcessError as e:
        return f"Error querying {domain}: {e}"
    except subprocess.TimeoutExpired:
        return f"Timeout querying {domain}"


# Function to load domains from a file
def load_domains(file_path):
    with open(file_path, 'r') as file:
        return [line.strip() for line in file if line.strip()]


# Function to execute concurrent queries
def concurrent_queries(domains, ip_address, max_workers, query_interval, num_queries=None):
    total_domains = len(domains)
    if num_queries is not None:
        total_domains = min(total_domains, num_queries)

    results = []
    timeouts = 0
    start_time = time.time()

    # Use tqdm to display a progress bar
    with tqdm(total=total_domains, desc="Pressure Testing") as pbar:
        # Use ThreadPoolExecutor for concurrent queries
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            # Submit query tasks to the thread pool
            futures = {executor.submit(query_domain, domain, ip_address): domain for domain in domains[:num_queries]}

            # Iterate over completed futures to get query results
            for future in as_completed(futures):
                domain = futures[future]
                try:
                    result = future.result()
                    results.append(result)
                except Exception as e:
                    results.append(f"Error querying {domain}: {e}")
                finally:
                    # Update the progress bar
                    pbar.update()
                    time.sleep(query_interval)

                    # Count timeouts
                    if "Timeout" in result:
                        timeouts += 1

                    # Calculate timeout rate and update description
                    timeout_rate = timeouts / len(results) if len(results) > 0 else 0.0
                    pbar.set_postfix({"Timeout": timeouts, "Timeout rate": f"{timeout_rate:.2%}", "Workers": max_workers})

    # Calculate final timeout rate
    timeout_rate = timeouts / total_domains if total_domains > 0 else 0.0
    end_time = time.time()
    elapsed_time = end_time - start_time

    # Calculate packet rate
    packet_rate = total_domains / elapsed_time if elapsed_time > 0 else 0.0

    print(f"Final Timeout rate: {timeout_rate:.2%}, Current workers: {max_workers}, Packet rate: {packet_rate:.2f} qps")

    return results


# Main program
if __name__ == "__main__":
    print("Queries started.")
    # Path to the domain file
    domain_file_path = 'data.txt'
    # IP address to query
    ip_address = '172.200.1.50'
    # Number of concurrent workers
    max_workers = 1
    # Delay between each query, in seconds
    query_interval = 0.5
    # Number of queries to perform (set to None to query all domains)
    num_queries = 10000

    # Load domains from file
    domains = load_domains(domain_file_path)

    # Execute concurrent queries
    results = concurrent_queries(domains, ip_address, max_workers, query_interval, num_queries)

    # Print completion message
    print("Queries completed.")
