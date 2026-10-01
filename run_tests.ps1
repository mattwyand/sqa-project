# run_tests.ps1
# Feeds every test input file to the front end and saves what the program prints.

$program = ".\frontend\frontend.exe"        # the program (built in Phase 2)
$runDir  = ".\TestResults\run_" + (Get-Date -Format "yyyy-MM-dd_HHmm")

# Find every input file in every folder under TestCases\Inputs
$inputs = Get-ChildItem -Path ".\TestCases\Inputs" -Recurse -Filter "*_input.txt"

foreach ($file in $inputs) {
    $testName = $file.Name -replace "_input.txt", ""
    $category = $file.Directory.Parent.Name

    # make a results folder for this category, e.g. TestResults\run_...\sell
    New-Item -ItemType Directory -Force -Path "$runDir\$category" | Out-Null

    # run the program using the input file, and save what it prints
    Get-Content $file.FullName | & $program > "$runDir\$category\${testName}_actual.txt"
}