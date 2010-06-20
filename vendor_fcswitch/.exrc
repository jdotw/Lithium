if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
map <NL> gqap
nnoremap  :bn
nnoremap  :bp
map  :shell
noremap   :call ToggleFold()
map ,cl :call InsertChangeLog()A
vmap ,dr :!tr A-Za-z N-ZA-Mn-za-m
map ,E 0/\$w"yy$:yr!
nmap ,cci :w:!astyle --style=ansi %:e!
nmap ,ci :w:!indent %:e!
vmap ," c""<Left>"
vmap ,) v`<i(`>a)
vmap ,( v`<i(`>a)
nmap ,j Vjgq
vmap ,fq :s/^> \([-":}|][ 	]\)/> > /
vmap ,kpq :s/^> *[a-zA-Z]*>/> >/
vmap ,qp :s/^/> /
nmap ,qp vip:s/^/> /
omap ,qp vip:s/^/> /
vmap ,dp :s/^> //
nmap ,dp vip:s/^> //
omap ,dp vip:s/^> //
map ,Sbl :g/^\s*$/,/\S/-j
map ,Sel :g/^$/,/./-j
vmap ,ksr :s/  \+/ /g
nmap ,ksr :%s/  \+/ /g
vmap ,cqel :s/^[>	 ]\+$//
nmap ,cqel :%s/^[>]\+$//
map ,del :g/^\s\+$/d
map ,cel :%s/^\s\+$//
vmap ,Stws :%s/  *$/_/g
nmap ,Stws :%s/  *$/_/g
vmap ,Sws :%s/ /_/g
nmap ,Sws :%s/ /_/g
nnoremap ,vy :edit   =myabbr
nnoremap ,ve :edit   =vimrc
nnoremap ,vu :source =vimrc
map -- :call AllIndent() 
nmap :Wq :wq
nmap :WQ :wq
nmap :wQ :wq
nmap :Q :q
nmap :W :w
map Q gq
nmap gx <Plug>NetrwBrowseX
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#NetrwBrowseX(expand("<cWORD>"),0)
map <F8> :bd
map <F7> :bn
map <F6> :bp
map <F5> :split
imap <NL> gqap
cmap ;rcm %s///g
map ð :cp
map î :cn
iabbr _PATH =expand("%:h")
iabbr _FILE =expand("%:t:r")
iabbr _PATHFILE =expand("%:p")
iabbr teh the
iabbr seperate separate
iabbr shoudl should
iabbr exmaple example
iabbr charcters characters
iabbr charcter character
iabbr aslo also
iabbr alos also
let &cpo=s:cpo_save
unlet s:cpo_save
set autowrite
set background=dark
set backspace=indent,eol,start
set cindent
set cinkeys=0{,0},!^F,o,O,e,*<Return>,0#
set cinoptions=>2,e0,n0,}0,^0,:2,=2,p2,t0,(2,)30,*40,+2,f0,{0,c2,u2
set comments=s1:/*,mb:*,el:*/,://,b:#,:%,:XCOMM,nb:>,fb:-
set complete=.,b,u,w,t,i,d
set dictionary=/usr/share/dict/words
set expandtab
set fileencodings=ucs-bom,utf-8,default,latin1
set formatoptions=croql
set helpheight=0
set helplang=en
set hidden
set highlight=8r,db,es,hs,mb,Mr,nu,rs,sr,tb,vr,ws
set history=100
set ignorecase
set incsearch
set iskeyword=@,48-57,_,192-255,-,.,@-@
set nojoinspaces
set keywordprg=dict
set laststatus=2
set lazyredraw
set modelines=4
set pastetoggle=<F4>
set path=.,~/
set report=0
set ruler
set shiftwidth=2
set showmatch
set smartcase
set smartindent
set smarttab
set softtabstop=2
set nostartofline
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
set tabstop=4
set textwidth=78
set tildeop
set title
set updatecount=100
set updatetime=6000
set viminfo=%,'20,\"500,:500,n~/.viminfo
set whichwrap=<,>,b,s,[,]
set window=0
set wrapmargin=3
" vim: set ft=vim :
