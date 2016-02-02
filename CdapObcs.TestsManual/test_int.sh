create namespace testspace
use namespace testspace
create stream test_int
truncate stream test_int
load stream test_int test_int.csv
set stream format test_int csv 'col_id int, col_int int, col_expected string'