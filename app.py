#!/usr/bin/env python3
import time
import json
import psutil
import random
import threading
from pprint import pprint
from flask_socketio import SocketIO
from flask import Flask, render_template

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app)

# Load configuration from JSON
with open('config.json') as config_file:
    CONFIG = json.load(config_file)
INCLUDED_PROCESSES = CONFIG.get("processes", [])


def get_process_data():
    processes = []
    pid_to_name = {}
    connections = []
    skip_pid = {}

    # Gather all processes with their PIDs and names
    for proc in psutil.process_iter(['pid', 'name', 'connections']):
        try:
            process_name = proc.info.get('name', "")
            if process_name in INCLUDED_PROCESSES:
                pid = proc.info['pid']
                pid_to_name[pid] = process_name
                processes.append({'label': process_name, 'id': pid})
        except (psutil.NoSuchProcess, psutil.AccessDenied):
            continue

    # Gather TCP connections for these processes
    for proc in psutil.process_iter(['pid', 'connections']):
        try:
            pid = proc.info['pid']
            if pid not in pid_to_name:
                continue  # Skip processes not in our target list

            for conn in proc.info['connections']:
                if conn.status == psutil.CONN_ESTABLISHED and conn.raddr:
                    if conn.laddr.port > conn.raddr.port:
                        continue
                    # Find the target PID by matching raddr with laddr of another process
                    target_pid = None
                    for target_proc in psutil.process_iter(['pid', 'connections']):
                        if target_proc.info['pid'] == pid:
                            continue  # Skip self
                        if target_proc.info['connections'] is None:
                            continue
                        for target_conn in target_proc.info['connections']:
                            if (target_conn.laddr.ip == conn.raddr.ip and
                                target_conn.laddr.port == conn.raddr.port):
                                target_pid = target_proc.info['pid']
                                break

                        if target_pid:
                            break

                    # Append connection if we have a valid target
                    if target_pid:
                        connections.append({
                            'source': pid,
                            'target': target_pid,
                            'port': conn.laddr.port
                        })

        except psutil.AccessDenied as e:
            pprint(e)
            continue
        except psutil.NoSuchProcess as e:
            pprint(e)
            continue

    return processes, connections


@app.route('/')
def index():
    return render_template('index.html')


@socketio.on('get_data')
def send_data():
    processes, connections = get_process_data()

    pprint(processes)
    pprint(connections)

    nodes = [{"data": {"id": proc["id"], "label": proc["label"]}}
             for proc in processes]
    edges = [
        {"data": {"source": conn["source"], "target": conn["target"], "label": f":{conn['port']}"}}
        for conn in connections
    ]

    socketio.emit('graph_data', {"nodes": nodes, "edges": edges})


# Start the Flask server
if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5001, debug=True)
