import subprocess
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from tqdm import tqdm
import streamlit as st
import numpy as np

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

    # Initialize lists to store the data for plotting
    timeout_rates_history = []
    packet_rates_history = []

    # Use tqdm to display a progress bar
    with tqdm(total=total_domains, desc="Pressure Testing") as pbar:
        # Use ThreadPoolExecutor for concurrent queries
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            # Submit query tasks to the thread pool
            futures = {executor.submit(query_domain, domain, ip_address): domain for domain in domains}

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

                    # Calculate elapsed time and packet rate
                    elapsed_time = time.time() - start_time
                    packet_rate = len(results) / elapsed_time if elapsed_time > 0 else 0.0

                    # Update timeout rate
                    if "Timeout" in result:
                        timeouts += 1
                    timeout_rate = timeouts / len(results) if len(results) > 0 else 0.0

                    # Store data for plotting
                    timeout_rates_history.append(timeout_rate)
                    packet_rates_history.append(packet_rate)

                    # Update the graphs every certain number of queries or time interval
                    if len(results) % 10 == 0 or (time.time() - start_time) > 1:
                        st.pyplot()  # This will trigger the plot update
                        st.line_chart(np.array(timeout_rates_history))
                        st.line_chart(np.array(packet_rates_history))

                    # Sleep for the specified query interval
                    time.sleep(query_interval)

    # Final update of the graphs
    st.pyplot()
    st.line_chart(np.array(timeout_rates_history))
    st.line_chart(np.array(packet_rates_history))

    return results

# Main program
if __name__ == "__main__":
    st.title("DNS 查询压力测试仪表板")
    domain_file_path = 'data.txt'
    ip_address = '172.200.1.50'
    max_workers = 10
    query_interval = 0.01
    num_queries = None

    # Load domains from file
    domains = load_domains(domain_file_path)

    # Execute concurrent queries and plot the graphs
    results = concurrent_queries(domains, ip_address, max_workers, query_interval, num_queries)

    st.write("查询完成。")