# Analysis for the inducing degeneration experiment

For the inducing degeneration experiment there are two important R scripts:
- analysis.R
- and max-abs-dev-N-trials.R


The first generates the analysis present in the article, and outputs it to the `analysis` folder in .txt format.

The second generates plots of the maximum absolute deviation of the centroid position as degeneration progresses for N randomly selected trials for each position and condition.
This means it generates 7 (different positions) * 5 (different conditions) = 35 plots - which can be accessed in `\plots\max-abs-dev-N-trials\{position_value}` the plots are generated in the `.gif` format and also in `.svg`.

N plots E {7 positions} * {5 conditions} => 35 .gifs and * 2 .svg (first and last frame are saved) with N plots.
(organised within positions - 5 plots per position)

---

Other analysis/scripts:

- avg max abs dev of centroid as degeneration progresses across all trials for each position and condition w/std error.
`avg-max-abs-dev.r` output `\plots\avg-max-abs-dev\{condition_name}` (kind of complete - lacks relevance?).
1 plot E {7 positions} * {5 conditions} => 35 figures with 1 plot (organised within conditions - 7 figures per conditions)
7 plots E {5 conditions} => 5 figures with 7 plots (at \plots\avg-max-abs-dev\) combined plot for better comparison.


- avg max abs deviation of centroid as degeneration progresses across all trials and across all positions for each condition w/ std error
`avg-max-abs-dev-avg-cond.r` output `\plots\avg-max-abs-dev-avg-cond`
5 plots in 1 figure {not done - lacks relevance?}