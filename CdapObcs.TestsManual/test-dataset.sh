create namespace testspace
use namespace testspace
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
