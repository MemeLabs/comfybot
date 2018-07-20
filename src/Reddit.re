type redditPostData = {url: string};

type redditPost = {
  data: redditPostData,
  kind: string,
};

type redditResponseData = {
  after: string,
  before: option(string),
  children: list(redditPost),
  modhash: string,
};

type redditResponse = {
  data: redditResponseData,
  kind: string,
};

let decodeRedditPostData = json =>
  Json.Decode.{url: json |> field("url", string)};

let decodeRedditPost = json : redditPost =>
  Json.Decode.{
    data: json |> field("data", decodeRedditPostData),
    kind: json |> field("kind", string),
  };

let decodeRedditResponseData = json : redditResponseData =>
  Json.Decode.{
    after: json |> field("after", string),
    before: json |> optional(field("before", string)),
    children: json |> field("children", list(decodeRedditPost)),
    modhash: json |> field("modhash", string),
  };

let decodeRedditResponse = json : redditResponse =>
  Json.Decode.{
    data: json |> field("data", decodeRedditResponseData),
    kind: json |> field("kind", string),
  };

let parseRedditResponse = json : redditResponse =>
  json |> Json.parseOrRaise |> decodeRedditResponse;

let fetchRedditPosts = () : Js.Promise.t(list(redditPost)) =>
  Fetch.fetch("https://www.reddit.com/r/cozyplaces.json")
  |> Js.Promise.then_(Fetch.Response.text)
  |> Js.Promise.then_(text =>
       switch (parseRedditResponse(text)) {
       | json => Js.Promise.resolve(json.data.children)
       | exception e => Js.Promise.reject(e)
       }
     );
