create namespace testspace
use namespace testspace
create stream test_timestamp
truncate stream test_timestamp
load stream test_timestamp test_timestamp.csv
set stream format test_timestamp csv 'col_id int, col_ts string'
execute 'select col_id, CAST(col_ts as TIMESTAMP), col_ts from stream_test_timestamp'