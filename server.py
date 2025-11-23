#!/usr/bin/env python3
"""
Simple HTTP server to run the Chess GUI
Run: python server.py
Then open http://localhost:8000 in your browser
"""

import http.server
import socketserver
import os
import webbrowser
import socket
from pathlib import Path

DIRECTORY = Path(__file__).parent

class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(DIRECTORY), **kwargs)

    def end_headers(self):
        self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
        super().end_headers()

def find_free_port(start_port=8000, max_attempts=10):
    """Find a free port starting from start_port"""
    for port in range(start_port, start_port + max_attempts):
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.bind(('', port))
                return port
        except OSError:
            continue
    return None

def run_server():
    PORT = find_free_port(8000)
    
    if PORT is None:
        print("Error: Could not find a free port!")
        return
    
    # Set socket option to allow reuse
    socketserver.TCPServer.allow_reuse_address = True
    
    try:
        with socketserver.TCPServer(("", PORT), MyHTTPRequestHandler) as httpd:
            url = f"http://localhost:{PORT}/gui.html"
            print(f"╔════════════════════════════════════════╗")
            print(f"║   Chess Engine GUI Server Started      ║")
            print(f"║   Open: {url:<33} ║")
            print(f"║   Press Ctrl+C to stop                 ║")
            print(f"╚════════════════════════════════════════╝\n")
            
            # Open browser automatically
            try:
                webbrowser.open(url)
            except:
                pass
            
            try:
                httpd.serve_forever()
            except KeyboardInterrupt:
                print("\n\nServer stopped.")
    except OSError as e:
        print(f"Error: {e}")
        print("Try closing other applications using port 8000 or run with a different port")

if __name__ == "__main__":
    run_server()
