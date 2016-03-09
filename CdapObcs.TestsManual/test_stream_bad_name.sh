create namespace testspace
use namespace testspace
delete stream test-bad-name
create stream test-bad-name
execute 'select * from stream_test_bad_name'
