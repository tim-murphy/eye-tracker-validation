# take a directory and write all CSV data to a new file

function printUsage
{
    $arg0 = Split-Path $PSCommandPath -Leaf
    echo "Usage: ${arg0} <data_dir> <out_csv>"
}

if ($args.count -lt 2)
{
    printUsage
    exit 1
}

$data_dir = $args[0]
$out_csv = $args[1]

Get-Content (Get-ChildItem $data_dir\*.csv) | Set-Content $out_csv
