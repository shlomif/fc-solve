next_idx = 0
File.open("find_optimal_quotas-300.dump", "r").each do |l| 
    if mo = /^Found (\d+) for No. (\d+) \((\d+)\)$/.match(l) then
        quota, idx, total_iters = mo[1], mo[2], mo[3]
        if (next_idx != idx.to_i) then
            raise "Idx #{idx} is non consecutive"
        end
        
        # puts "#{idx}\t#{total_iters}"
        puts quota

        next_idx += 1
    end
end

