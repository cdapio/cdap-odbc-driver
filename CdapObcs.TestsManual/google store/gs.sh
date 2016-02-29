use namespace default
delete stream gs-orders
create stream gs-orders
load stream gs-orders gs-orders.tsv
set stream format gs-orders tsv "row_id long, order_id string, order_date string, ship_date string, ship_mode string, customer_id string, customer_name string, segment string, city string, state string, country string, postal_code string, market string, region string, product_id string, category string, sub_category string, product_name string, sales double, quantity long, discount double, profit double, shipping_cost double, order_priority string"
execute 'select * from stream_gs_orders limit 5'
delete stream gs-returns
create stream gs-returns
load stream gs-returns gs-returns.tsv
set stream format gs-returns tsv "returned string, order_id string, market string"
execute 'select * from stream_gs_returns limit 5'
delete stream gs-people
create stream gs-people
load stream gs-people gs-people.tsv
set stream format gs-people tsv "person string, region string"
execute 'select * from stream_gs_people limit 5'
