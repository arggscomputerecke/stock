$stockNames = gc .\stockNames.data
$stockNames | foreach{  
    $name= ($_.Split(':;"')[6]); 
    Write-Output $name
    Invoke-WebRequest -uri "http://ichart.finance.yahoo.com/table.csv?s=$name&a=4&b=25&c=2011&d=2&e=31&f=2017&g=d&ignore=.csv" -OutFile "$name.txt"
 }