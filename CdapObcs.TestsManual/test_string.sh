create namespace testspace
use namespace testspace
create stream test_string
truncate stream test_string
load stream test_string test_string.csv
set stream format test_string csv 'col_id int, col_string string, col_expected string'
execute 'select col_id, substr(col_string,0,100), length(col_string), col_expected from stream_test_string'