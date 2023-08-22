name: Mark Discussions as Solved
on:
  schedule:
    # Schedule this action to run periodically (e.g., daily)
    - cron: '0 0 * * *'

jobs:
  mark-as-solved:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout code
        uses: actions/checkout@v2

      - name: Set up Node.js
        uses: actions/setup-node@v2
        with:
          node-version: '14'

      - name: Install dependencies
        run: npm install @actions/github octokit/rest.js

      - name: Find and Mark Discussions as Solved
        run: |
          # Import necessary modules
          const { Octokit } = require("@octokit/rest");
          const githubToken = process.env.GITHUB_TOKEN;
          const octokit = new Octokit({ auth: githubToken });

          // Define the team name to check against
          const teamName = "Reviewers";

          // Calculate one year ago from the current date
          const oneYearAgo = new Date();
          oneYearAgo.setFullYear(oneYearAgo.getFullYear() - 1);

          // Fetch discussions in the repository
          const repoOwner = "sofa-framework";
          const repoName = "sofa";
          const discussionsResponse = await octokit.rest.issues.list({
            owner: repoOwner,
            repo: repoName,
            q: `is:discussion is:open`,
          });

          // Process discussions
          for (const discussion of discussionsResponse.data.items) {
            // Get the oldest reply
            const repliesResponse = await octokit.rest.issues.listComments({
              owner: repoOwner,
              repo: repoName,
              issue_number: discussion.number,
            });

            if (repliesResponse.data.length > 0) {
              const oldestReply = repliesResponse.data[0];

              // Check if the oldest reply is by a team member and is at least one year old
              if (
                oldestReply.user.type === "Team" &&
                oldestReply.user.login === teamName &&
                new Date(oldestReply.created_at) <= oneYearAgo
              ) {
                // Print discussion title, ID, and the name of the team member
                console.log(`Discussion Title: ${discussion.title}`);
                console.log(`Discussion ID: ${discussion.id}`);
                console.log(`Last Reply by ${teamName} Member: ${lastReplyUserName}`);

                // Mark the discussion as solved
                //await octokit.rest.reactions.createForIssueComment({
                //  owner: repoOwner,
                //  repo: repoName,
                //  comment_id: oldestReply.id,
                //  content: "+1", // You can use a different reaction if desired
                //});

                // Add a comment "solved"
                //await octokit.rest.issues.createComment({
                //  owner: repoOwner,
                //  repo: repoName,
                //  issue_number: discussion.number,
                //  body: "This discussion has been automatically solved due to inactivity.<br>Do not hesitate to re-open the discussion if this is not solved and share with us any update!",
                //});
              }
            }
          }
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
