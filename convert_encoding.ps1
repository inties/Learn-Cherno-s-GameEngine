# 批量转换文件编码脚本
# C++ 文件转为 GB2312
Write-Host "Converting C++ files to GB2312..."
Get-ChildItem -Recurse -Include *.cpp,*.h,*.hpp,*.c | ForEach-Object {
    try {
        $content = Get-Content $_.FullName -Raw
        if ($content) {
            [System.IO.File]::WriteAllText($_.FullName, $content, [System.Text.Encoding]::GetEncoding("GB2312"))
            Write-Host "Converted: $($_.FullName)"
        }
    }
    catch {
        Write-Host "Error converting $($_.FullName): $($_.Exception.Message)" -ForegroundColor Red
    }
}

# Python 文件转为 UTF-8
Write-Host "Converting Python files to UTF-8..."
Get-ChildItem -Recurse -Include *.py | ForEach-Object {
    try {
        $content = Get-Content $_.FullName -Raw
        if ($content) {
            [System.IO.File]::WriteAllText($_.FullName, $content, [System.Text.Encoding]::UTF8)
            Write-Host "Converted: $($_.FullName)"
        }
    }
    catch {
        Write-Host "Error converting $($_.FullName): $($_.Exception.Message)" -ForegroundColor Red
    }
}

Write-Host "Conversion completed!" 