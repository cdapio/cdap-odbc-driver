create namespace testspace
use namespace testspace

create stream test_boolean
truncate stream test_boolean
load stream test_boolean test_boolean.csv
set stream format test_boolean csv 'col_id int, col_boolean boolean, col_expected string'
execute 'select col_id, col_boolean, col_expected from stream_test_boolean'

create stream test_binary
truncate stream test_binary
load stream test_binary test_binary.csv
set stream format test_binary csv 'col_value bytes'
execute 'select col_value from stream_test_binary'

create stream test_double
truncate stream test_double
load stream test_double test_double.csv
set stream format test_double csv 'col_id int, col_double double, col_expected string'
execute 'select col_id, col_double, col_expected from stream_test_double'

create stream test_float
truncate stream test_float
load stream test_float test_float.csv
set stream format test_float csv 'col_id int, col_float float, col_expected string'
execute 'select col_id, col_float, col_expected from stream_test_float'

create stream test_int
truncate stream test_int
load stream test_int test_int.csv
set stream format test_int csv 'col_id int, col_int int, col_expected string'
execute 'select col_id, col_int, col_expected from stream_test_int'

create stream test_long
truncate stream test_long
load stream test_long test_long.csv
set stream format test_long csv 'col_id int, col_long long, col_expected string'
execute 'select col_id, col_long, col_expected from stream_test_long'

create stream test_string
truncate stream test_string
load stream test_string test_string.csv
set stream format test_string csv 'col_id int, col_string string, col_expected string'
execute 'select col_id, col_string, col_expected from stream_test_string'

create stream test-bad-name
truncate stream test-bad-name
execute 'select * from stream_test_bad_name'

stop flow TestDataset.WhoFlow
delete artifact TestDataset 3.2.2
truncate stream streamTestDataset
delete stream streamTestDataset
delete dataset instance dsTest
delete dataset instance dsTest-Bad-Name
load artifact TestDataset-3.2.2.jar
create app TestDataset TestDataset 3.2.2 user
start flow TestDataset.StreamFlow
describe dataset instance dsTest
describe dataset instance dsTest-Bad-Name
send stream streamTestDataset "event1"
send stream streamTestDataset "event2"
send stream streamTestDataset "event3"
send stream streamTestDataset "event4"
send stream streamTestDataset "event5"
execute 'select * from dataset_dsTest'
execute 'select * from dataset_dsTest_Bad_Name'

create stream items
truncate stream items
load stream items test-items.csv
set stream format items csv "id int not null, name string not null, price float not null"
execute 'select * from stream_items'
create stream orders
truncate stream orders
load stream orders test-orders.csv
set stream format orders csv "order_id int not null, item_id int not null, quantity int not null"
execute 'select * from stream_orders'
create stream waiters
truncate stream waiters
load stream waiters test-waiters.csv
set stream format waiters csv "id int not null, name string not null, department string, manager int"

execute 'select * from stream_waiters'
execute 'select stream_orders.order_id as order_id, stream_items.name as name, stream_orders.quantity as quantity from stream_items, stream_orders where stream_items.id=stream_orders.item_id order by order_id'

create stream test_date
truncate stream test_date
load stream test_date test_date.csv
set stream format test_date csv 'col_id int, col_date string'
execute 'select col_id, CAST(col_date as DATE), col_date from stream_test_date'

create stream test_timestamp
truncate stream test_timestamp
load stream test_timestamp test_timestamp.csv
set stream format test_timestamp csv 'col_id int, col_ts string'
execute 'select col_id, CAST(col_ts as TIMESTAMP), col_ts from stream_test_timestamp'
