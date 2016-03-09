create namespace testspace
use namespace testspace

delete stream items
create stream items
load stream items test-items.csv
set stream format items csv "id int not null, name string not null, price float not null"
execute 'select * from stream_items'
delete stream orders
create stream orders
load stream orders test-orders.csv
set stream format orders csv "order_id int not null, item_id int not null, quantity int not null"
execute 'select * from stream_orders'
create stream waiters
load stream waiters test-waiters.csv
set stream format waiters csv "id int not null, name string not null, department string, manager int"

execute 'select * from stream_waiters'
execute 'select stream_orders.order_id as order_id, stream_items.name as name, stream_orders.quantity as quantity from stream_items, stream_orders where stream_items.id=stream_orders.item_id order by order_id'