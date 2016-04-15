create namespace testspace
use namespace testspace
create stream test_long
truncate stream test_long
load stream test_long test_long.csv
set stream format test_long csv 'col_id int, col_long long, col_expected string'
execute 'select col_id, col_long, col_expected from stream_test_long'