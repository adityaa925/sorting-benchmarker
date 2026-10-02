#!/usr/bin/env python3
"""
Dashboard Report Generator
Reads benchmark results JSON and merges it into the Chart.js interactive HTML template.
"""

import argparse
import json
import os
import sys

def generate_report(json_path: str, template_path: str, output_path: str) -> None:
    if not os.path.exists(json_path):
        print(f"Error: JSON results file not found: {json_path}", file=sys.stderr)
        sys.exit(1)

    if not os.path.exists(template_path):
        print(f"Error: HTML template not found: {template_path}", file=sys.stderr)
        sys.exit(1)

    with open(json_path, 'r', encoding='utf-8') as f:
        try:
            data = json.load(f)
            json_str = json.dumps(data)
        except Exception as e:
            print(f"Error reading JSON: {e}", file=sys.stderr)
            sys.exit(1)

    with open(template_path, 'r', encoding='utf-8') as f:
        template_content = f.read()

    # Replace placeholder with embedded json data
    target_placeholder = "/*{{BENCHMARK_DATA}}*/ null"
    if target_placeholder in template_content:
        html_content = template_content.replace(target_placeholder, json_str)
    elif "/*{{BENCHMARK_DATA}}*/" in template_content:
        html_content = template_content.replace("/*{{BENCHMARK_DATA}}*/", json_str)
    else:
        # Fallback insertion
        html_content = template_content.replace("let rawData =", f"let rawData = {json_str}; //")

    os.makedirs(os.path.dirname(os.path.abspath(output_path)), exist_ok=True)
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(html_content)

    print(f"[OK] Generated interactive benchmark dashboard: {output_path}")

def main():
    parser = argparse.ArgumentParser(description="Generate interactive HTML report from sorting benchmark JSON.")
    parser.add_argument("--input", "-i", default="benchmark_results.json", help="Path to benchmark JSON results file")
    parser.add_argument("--template", "-t", default=None, help="Path to dashboard.html template")
    parser.add_argument("--output", "-o", default="report.html", help="Path for generated HTML output file")

    args = parser.parse_args()

    # Locate template if not explicitly specified
    template_path = args.template
    if not template_path:
        script_dir = os.path.dirname(os.path.abspath(__file__))
        candidate_1 = os.path.join(script_dir, "template", "dashboard.html")
        candidate_2 = os.path.join("visualization", "template", "dashboard.html")
        if os.path.exists(candidate_1):
            template_path = candidate_1
        elif os.path.exists(candidate_2):
            template_path = candidate_2
        else:
            template_path = candidate_1

    generate_report(args.input, template_path, args.output)

if __name__ == "__main__":
    main()
