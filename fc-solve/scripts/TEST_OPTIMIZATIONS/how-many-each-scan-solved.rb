require 'getoptlong'

opts = GetoptLong.new(
    [ '--theme', GetoptLong::REQUIRED_ARGUMENT ],
    [ '--iters', GetoptLong::REQUIRED_ARGUMENT ],
);

theme_fn = ''
iters_fn = ''

opts.each do |opt, arg|
    case opt
    when '--theme'
        theme_fn = arg
    when '--iters'
        iters_fn = arg
    end
end

quotas = Array.new
File.open(theme_fn, "r") do |file|
    while (l = file.gets)
        if (m = /--prelude "([^"]+)"/.match(l))
            prelude = m[1]
            quotas = prelude.split(/,/).map { |x|
                if (m = /\A([0-9]+)@(\w+)\z/.match(x))
                    { :iters => m[1].to_i, :scan => m[2], :found => 0 }
                else
                    throw "Foo"
                end
            }
        end
    end
end

start_at = 0
found_quotas = Array.new
File.open(iters_fn, "r") do |file|
    while (l = file.gets)
        if (m = /\A\s*([0-9]+)\s+(-?[0-9]+)\s*\z/.match(l))
            scan = m[1].to_i;
            iters = m[2].to_i;

            if (iters >= 0)
                while (iters > start_at + quotas[0][:iters])
                    start_at += quotas[0][:iters]
                    found_quotas << quotas.shift
                end
                quotas[0][:found] += 1
            end
        end
    end
end

for q_s in [found_quotas, quotas] do
    for q in q_s do
        puts "#{q[:scan]}\t#{q[:found]}"
    end
end
