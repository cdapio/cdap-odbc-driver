create namespace testspace
use namespace testspace
create stream test_binary
truncate stream test_binary
load stream test_binary test_binary.csv
set stream format test_binary csv 'col_value binary'
execute 'select col_value from stream_test_binary'