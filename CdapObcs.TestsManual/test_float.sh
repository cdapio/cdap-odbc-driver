create namespace testspace
use namespace testspace
create stream test_float
truncate stream test_float
load stream test_float test_float.csv
set stream format test_float csv 'col_id int, col_float float, col_expected string'
execute 'select col_id, col_float, col_expected from stream_test_float'