create namespace testspace
use namespace testspace
create stream test_float
truncate stream test_float
load stream test_float test_float.csv
set stream format test_float csv 'col_id int, col_float float, col_expected string'