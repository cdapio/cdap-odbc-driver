create namespace testspace
use namespace testspace
create stream test_double
truncate stream test_double
load stream test_double test_double.csv
set stream format test_double csv 'col_id int, col_double double, col_expected string'