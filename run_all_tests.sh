#!/bin/bash

OUTPUT_FILE="test_results.txt"
TESTS_DIR="tests"

> "$OUTPUT_FILE"

echo "=====================================" | tee -a "$OUTPUT_FILE"
echo "Running all .asm tests" | tee -a "$OUTPUT_FILE"
echo "Date: $(date)" | tee -a "$OUTPUT_FILE"
echo "=====================================" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

# Conta o total de testes
total_tests=$(find "$TESTS_DIR" -name "*.asm" | wc -l | tr -d ' ')
echo "Found $total_tests test files" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"

# Contadores
passed=0
failed=0

# Itera sobre todos os arquivos .asm na pasta tests
for asm_file in "$TESTS_DIR"/*.asm; do
    if [ -f "$asm_file" ]; then
        # Pega apenas o nome do arquivo
        filename=$(basename "$asm_file")
        
        echo "----------------------------------------" | tee -a "$OUTPUT_FILE"
        echo "Testing: $filename" | tee -a "$OUTPUT_FILE"
        echo "----------------------------------------" | tee -a "$OUTPUT_FILE"
        
        # Roda o compilador e captura o output
        output=$(./compiler "$asm_file" 2>&1)
        exit_code=$?
        
        # Salva o output no arquivo
        if [ -z "$output" ]; then
            echo "✓ SUCCESS - No errors" | tee -a "$OUTPUT_FILE"
            ((passed++))
        else
            echo "✗ ERRORS/WARNINGS FOUND:" | tee -a "$OUTPUT_FILE"
            echo "$output" | tee -a "$OUTPUT_FILE"
            ((failed++))
        fi
        
        echo "" | tee -a "$OUTPUT_FILE"
    fi
done

# Sumário final
echo "=====================================" | tee -a "$OUTPUT_FILE"
echo "SUMMARY" | tee -a "$OUTPUT_FILE"
echo "=====================================" | tee -a "$OUTPUT_FILE"
echo "Total tests: $total_tests" | tee -a "$OUTPUT_FILE"
echo "Passed: $passed" | tee -a "$OUTPUT_FILE"
echo "Failed: $failed" | tee -a "$OUTPUT_FILE"
echo "" | tee -a "$OUTPUT_FILE"
echo "Results saved to: $OUTPUT_FILE" | tee -a "$OUTPUT_FILE"
