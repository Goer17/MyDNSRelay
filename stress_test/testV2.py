import subprocess
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
import threading
import streamlit as st
import plotly.graph_objects as go

# Global variables to store results and metrics
results = []
packet_rate_data = []
timeout_rate_data = []
time_data = []
start_time = 0
total_domains = 0
timeout_count = 0
stop_event = threading.Event()
query_started = False


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
    global results, packet_rate_data, timeout_rate_data, time_data, start_time, total_domains, timeout_count
    total_domains = len(domains)
    if num_queries is not None:
        total_domains = min(total_domains, num_queries)

    results = []
    timeout_count = 0
    start_time = time.time()

    packet_rate_data = []
    timeout_rate_data = []
    time_data = []

    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = {executor.submit(query_domain, domain, ip_address): domain for domain in domains[:num_queries]}
        for future in as_completed(futures):
            if stop_event.is_set():
                break
            domain = futures[future]
            try:
                result = future.result()
                results.append(result)
            except Exception as e:
                results.append(f"Error querying {domain}: {e}")
            finally:
                time.sleep(query_interval)
                if "Timeout" in result:
                    timeout_count += 1
                current_time = time.time() - start_time
                packet_rate = len(results) / current_time if current_time > 0 else 0.0
                timeout_rate = timeout_count / len(results) if len(results) > 0 else 0.0
                packet_rate_data.append(packet_rate)
                timeout_rate_data.append(timeout_rate)
                time_data.append(current_time)


def start_queries(ip_address, query_interval, max_workers, num_queries):
    global stop_event, query_started
    stop_event.set()  # Stop any ongoing queries
    stop_event.clear()
    domains = load_domains('data.txt')
    query_started = True
    threading.Thread(target=concurrent_queries,
                     args=(domains, ip_address, max_workers, query_interval, num_queries)).start()


# Streamlit app layout
st.title("DNS Pressure Test")

# Sidebar for inputs
st.sidebar.title("Settings")
ip_address = st.sidebar.text_input("DNS Relay IP Address", "172.200.1.50")
query_interval = st.sidebar.number_input("Query Interval (seconds)", value=0.5, min_value=0.1, max_value=10.0, step=0.1)
max_workers = st.sidebar.number_input("Max Workers", value=1, min_value=1, max_value=10, step=1)
num_queries = st.sidebar.number_input("Number of Queries", value=10, min_value=1, max_value=100, step=1)
start_button = st.sidebar.button("Start")

# Main content area for the graph
st.sidebar.title("Results")

if start_button:
    start_queries(ip_address, query_interval, max_workers, num_queries)

# Create empty placeholders for the graph and metrics
graph_placeholder = st.empty()
metrics_placeholder = st.empty()

# Progress bar initialization
progress_bar = st.sidebar.progress(0)

# Real-time update of the graph and metrics
while query_started and len(results) < num_queries:
    if stop_event.is_set():
        break
    time.sleep(1)  # Ensure updates every 1 second
    if len(time_data) > 0:
        progress = min(len(results) / num_queries, 1.0)
        progress_bar.progress(progress)

        fig = go.Figure()
        fig.add_trace(go.Scatter(x=time_data, y=packet_rate_data, mode='lines', name='Speed (qps)'))
        fig.add_trace(go.Scatter(x=time_data, y=timeout_rate_data, mode='lines', name='Timeout Rate'))

        fig.update_layout(
            title='Queries Speed and Timeout Rate Over Time',
            xaxis_title='Time (seconds)',
            yaxis_title='Rate',
            legend_title='Rate Type',
            plot_bgcolor='#0E1117',
            paper_bgcolor='#0E1117',
            font=dict(color='white')
        )

        fig.update_layout(width=800, height=600)
        graph_placeholder.plotly_chart(fig, use_container_width=True)

        # Calculate and display packet rate and timeout rate
        total_time = time_data[-1]
        packet_rate = len(results) / total_time if total_time > 0 else 0.0
        timeout_rate = timeout_count / len(results) if len(results) > 0 else 0.0
        elapsed_time = time.strftime("%H:%M:%S", time.gmtime(total_time))

        # Update metrics
        metrics_placeholder.empty()
        metrics_placeholder.text(f"Total Queries: {len(results)}\n"
                                 f"Timeouts: {timeout_count}\n"
                                 f"Speed: {packet_rate:.2f} qps\n"
                                 f"Timeout Rate: {timeout_rate:.2%}\n"
                                 f"Run Time: {elapsed_time}")

# Only show the final counts after queries are completed
if query_started and len(results) >= num_queries:
    st.sidebar.text(f"Total Queries: {len(results)}")
    st.sidebar.text(f"Timeouts: {timeout_count}")

    # Calculate and display packet rate and timeout rate
    if len(time_data) > 0:
        total_time = time_data[-1]
        packet_rate = len(results) / total_time if total_time > 0 else 0.0
        timeout_rate = timeout_count / len(results) if len(results) > 0 else 0.0
        elapsed_time = time.strftime("%H:%M:%S", time.gmtime(total_time))
        st.sidebar.text(f"Speed: {packet_rate:.2f} qps")
        st.sidebar.text(f"Timeout Rate: {timeout_rate:.2%}")
        st.sidebar.text(f"Run Time: {elapsed_time}")

    query_started = False
