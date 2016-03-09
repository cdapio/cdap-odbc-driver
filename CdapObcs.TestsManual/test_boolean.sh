create namespace testspace
use namespace testspace
create stream test_boolean
truncate stream test_boolean
load stream test_boolean test_boolean.csv
set stream format test_boolean csv 'col_id int, col_boolean boolean, col_expected string'
execute 'select col_id, col_boolean, col_expected from stream_test_boolean'