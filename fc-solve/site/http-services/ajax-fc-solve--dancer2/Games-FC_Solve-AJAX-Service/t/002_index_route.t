use Test::More tests => 1;
use strict;
use warnings;

# the order is important
use Plack::Test;
use HTTP::Request::Common;
use Games::FC_Solve::AJAX::Service;

my $app  = Games::FC_Solve::AJAX::Service->to_app;
my $test = Plack::Test->create($app);
my $res  = $test->request( GET '/' );

# TEST
is $res->code, 200, 'response status is 200 for /';
