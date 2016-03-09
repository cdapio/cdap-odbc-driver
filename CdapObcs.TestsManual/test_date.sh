create namespace testspace
use namespace testspace
create stream test_date
truncate stream test_date
load stream test_date test_date.csv
set stream format test_date csv 'col_id int, col_date string'
execute 'select col_id, CAST(col_date as DATE), col_date from stream_test_date'