package Games::FC_Solve::AJAX::Service;
use Dancer ':syntax';

use Dancer::Plugin::REST;

prepare_serializer_for_format;
our $VERSION = '0.1';

get '/' => sub {
    template 'index';
};

get '/run-fc-solve/idx=:idx/preset=:preset.:format' => sub {
    my $idx = params->{idx};
    if ($idx !~ /\A[0-9]+\z/)
    {
        die "Incorrect index.";
    }

    my $preset = params->{preset};
    if ($preset ne 'as')
    {
        die "Unknown preset.";
    }

    return
    +{
        ret_text => scalar(
            qx(pi-make-microsoft-freecell-board -t "$idx" | fc-solve -l "$preset" -sam -p -t -sel -mi 300000 | expand-solitaire-multi-card-moves | perl -lane 's/ +\$//; print unless /^This game is solv/..1')
        ),
    };
};

true;
